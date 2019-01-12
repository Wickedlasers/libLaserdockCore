/*
  Copyright (C) 2012-2014 Paul Brossier <piem@aubio.org>

  This file is part of aubio.

  aubio is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  aubio is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with aubio.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "aubio_priv.h"

#ifdef HAVE_SINK_APPLE_AUDIO
#include "fvec.h"
#include "fmat.h"
#include "io/sink_apple_audio.h"
#include "io/ioutils.h"

// CFURLRef, CFURLCreateWithFileSystemPath, ...
#include <CoreFoundation/CoreFoundation.h>
// ExtAudioFileRef, AudioStreamBasicDescription, AudioBufferList, ...
#include <AudioToolbox/AudioToolbox.h>

#define FLOAT_TO_SHORT(x) (short)(x * 32768)

extern int createAubioBufferList(AudioBufferList *bufferList, int channels, int segmentSize);
extern void freeAudioBufferList(AudioBufferList *bufferList);
extern CFURLRef createURLFromPath(const char * path);
char_t *getPrintableOSStatusError(char_t *str, OSStatus error);

uint_t aubio_sink_apple_audio_open(aubio_sink_apple_audio_t *s);

#define MAX_SIZE 4096 // the maximum number of frames that can be written at a time

void aubio_sink_apple_audio_write(aubio_sink_apple_audio_t *s, uint_t write);

struct _aubio_sink_apple_audio_t {
  uint_t samplerate;
  uint_t channels;
  char_t *path;

  uint_t max_frames;

  AudioBufferList bufferList;
  ExtAudioFileRef audioFile;
  bool async;
};

aubio_sink_apple_audio_t * new_aubio_sink_apple_audio(const char_t * uri, uint_t samplerate) {
  aubio_sink_apple_audio_t * s = AUBIO_NEW(aubio_sink_apple_audio_t);
  s->max_frames = MAX_SIZE;
  s->async = false;

  if ( (uri == NULL) || (strlen(uri) < 1) ) {
    AUBIO_ERROR("sink_apple_audio: Aborted opening null path\n");
    goto beach;
  }
  if (s->path != NULL) AUBIO_FREE(s->path);
  s->path = AUBIO_ARRAY(char_t, strnlen(uri, PATH_MAX) + 1);
  strncpy(s->path, uri, strnlen(uri, PATH_MAX) + 1);

  s->samplerate = 0;
  s->channels = 0;

  // zero samplerate given. do not open yet
  if ((sint_t)samplerate == 0) {
    return s;
  }
  // invalid samplerate given, abort
  if (aubio_io_validate_samplerate("sink_apple_audio", s->path, samplerate)) {
    goto beach;
  }

  s->samplerate = samplerate;
  s->channels = 1;

  if (aubio_sink_apple_audio_open(s) != AUBIO_OK) {
    // open failed, abort
    goto beach;
  }

  return s;
beach:
  AUBIO_FREE(s);
  return NULL;
}

uint_t aubio_sink_apple_audio_preset_samplerate(aubio_sink_apple_audio_t *s, uint_t samplerate)
{
  if (aubio_io_validate_samplerate("sink_apple_audio", s->path, samplerate)) {
    return AUBIO_FAIL;
  }
  s->samplerate = samplerate;
  // automatically open when both samplerate and channels have been set
  if (s->samplerate != 0 && s->channels != 0) {
    return aubio_sink_apple_audio_open(s);
  }
  return AUBIO_OK;
}

uint_t aubio_sink_apple_audio_preset_channels(aubio_sink_apple_audio_t *s, uint_t channels)
{
  if (aubio_io_validate_channels("sink_apple_audio", s->path, channels)) {
    return AUBIO_FAIL;
  }
  s->channels = channels;
  // automatically open when both samplerate and channels have been set
  if (s->samplerate != 0 && s->channels != 0) {
    return aubio_sink_apple_audio_open(s);
  }
  return AUBIO_OK;
}

uint_t aubio_sink_apple_audio_get_samplerate(const aubio_sink_apple_audio_t *s)
{
  return s->samplerate;
}

uint_t aubio_sink_apple_audio_get_channels(const aubio_sink_apple_audio_t *s)
{
  return s->channels;
}

uint_t aubio_sink_apple_audio_open(aubio_sink_apple_audio_t *s) {

  if (s->samplerate == 0 || s->channels == 0) return AUBIO_FAIL;

  AudioStreamBasicDescription clientFormat;
  memset(&clientFormat, 0, sizeof(AudioStreamBasicDescription));
  clientFormat.mFormatID         = kAudioFormatLinearPCM;
  clientFormat.mSampleRate       = (Float64)(s->samplerate);
  clientFormat.mFormatFlags      = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
  clientFormat.mChannelsPerFrame = s->channels;
  clientFormat.mBitsPerChannel   = sizeof(short) * 8;
  clientFormat.mFramesPerPacket  = 1;
  clientFormat.mBytesPerFrame    = clientFormat.mBitsPerChannel * clientFormat.mChannelsPerFrame / 8;
  clientFormat.mBytesPerPacket   = clientFormat.mFramesPerPacket * clientFormat.mBytesPerFrame;
  clientFormat.mReserved         = 0;

  AudioFileTypeID fileType = kAudioFileWAVEType;
  CFURLRef fileURL = createURLFromPath(s->path);
  bool overwrite = true;
  OSStatus err = noErr;
  err = ExtAudioFileCreateWithURL(fileURL, fileType, &clientFormat, NULL,
     overwrite ? kAudioFileFlags_EraseFile : 0, &s->audioFile);
  CFRelease(fileURL);
  if (err) {
    char_t errorstr[20];
    AUBIO_ERR("sink_apple_audio: error when trying to create %s with "
        "ExtAudioFileCreateWithURL (%s)\n", s->path,
        getPrintableOSStatusError(errorstr, err));
    goto beach;
  }
  if (createAubioBufferList(&s->bufferList, s->channels, s->max_frames * s->channels)) {
    AUBIO_ERR("sink_apple_audio: error when creating buffer list for %s, "
        "out of memory? \n", s->path);
    goto beach;
  }
  return AUBIO_OK;

beach:
  return AUBIO_FAIL;
}

void aubio_sink_apple_audio_do(aubio_sink_apple_audio_t * s, fvec_t * write_data, uint_t write) {
  UInt32 c, v;
  short *data = (short*)s->bufferList.mBuffers[0].mData;
  if (write > s->max_frames) {
    AUBIO_WRN("sink_apple_audio: trying to write %d frames, max %d\n", write, s->max_frames);
    write = s->max_frames;
  }
  smpl_t *buf = write_data->data;

  if (buf) {
      for (c = 0; c < s->channels; c++) {
          for (v = 0; v < write; v++) {
              data[v * s->channels + c] =
                  FLOAT_TO_SHORT(buf[ v * s->channels + c]);
          }
      }
  }
  aubio_sink_apple_audio_write(s, write);
}

void aubio_sink_apple_audio_do_multi(aubio_sink_apple_audio_t * s, fmat_t * write_data, uint_t write) {
  UInt32 c, v;
  short *data = (short*)s->bufferList.mBuffers[0].mData;
  if (write > s->max_frames) {
    AUBIO_WRN("sink_apple_audio: trying to write %d frames, max %d\n", write, s->max_frames);
    write = s->max_frames;
  }
  smpl_t **buf = write_data->data;

  if (buf) {
      for (c = 0; c < s->channels; c++) {
          for (v = 0; v < write; v++) {
              data[v * s->channels + c] =
                  FLOAT_TO_SHORT(buf[c][v]);
          }
      }
  }
  aubio_sink_apple_audio_write(s, write);
}

void aubio_sink_apple_audio_write(aubio_sink_apple_audio_t *s, uint_t write) {
  OSStatus err = noErr;
  if (s->async) {
    err = ExtAudioFileWriteAsync(s->audioFile, write, &s->bufferList);
    if (err) {
      char_t errorstr[20];
      if (err == kExtAudioFileError_AsyncWriteBufferOverflow) {
        sprintf(errorstr,"buffer overflow");
      } else if (err == kExtAudioFileError_AsyncWriteTooLarge) {
        sprintf(errorstr,"write too large");
      } else {
        // unknown error
        getPrintableOSStatusError(errorstr, err);
      }
      AUBIO_ERROR("sink_apple_audio: error while writing %s "
                  "in ExtAudioFileWriteAsync (%s)\n", s->path, errorstr);
    }
  } else {
    err = ExtAudioFileWrite(s->audioFile, write, &s->bufferList);
    if (err) {
      char_t errorstr[20];
      AUBIO_ERROR("sink_apple_audio: error while writing %s "
          "in ExtAudioFileWrite (%s)\n", s->path,
          getPrintableOSStatusError(errorstr, err));
    }
  }
}

uint_t aubio_sink_apple_audio_close(aubio_sink_apple_audio_t * s) {
  OSStatus err = noErr;
  if (!s->audioFile) {
    return AUBIO_FAIL;
  }
  err = ExtAudioFileDispose(s->audioFile);
  if (err) {
    char_t errorstr[20];
    AUBIO_ERROR("sink_apple_audio: error while closing %s "
        "in ExtAudioFileDispose (%s)\n", s->path,
        getPrintableOSStatusError(errorstr, err));
  }
  s->audioFile = NULL;
  return err;
}

void del_aubio_sink_apple_audio(aubio_sink_apple_audio_t * s) {
  if (s->audioFile) aubio_sink_apple_audio_close (s);
  if (s->path) AUBIO_FREE(s->path);
  freeAudioBufferList(&s->bufferList);
  AUBIO_FREE(s);
  return;
}

#endif /* HAVE_SINK_APPLE_AUDIO */
