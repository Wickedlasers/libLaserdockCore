#include "OpenSLDecoder.hpp"
#include "AndroidLogging.h"
#include <fcntl.h>
#include <fstream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <assert.h>

#define PREFETCH_ERROR_MASK (SL_PREFETCHEVENT_STATUSCHANGE | SL_PREFETCHEVENT_FILLLEVELCHANGE)
// used to detect errors likely to have occured when the OpenSL ES framework fails to open
// a resource, for instance because a file URI is invalid, or an HTTP server doesn't respond.

void OpenSLDecoder::setInput(std::string inputPath)
{
    this->inputPath = inputPath;
}

void OpenSLDecoder::setListener(OpenSLDecoderListener *listener)
{
    this->listener = listener;
}

bool OpenSLDecoder::start()
{
    SLresult res;
    duration = SL_TIME_UNKNOWN;
    progress = 0;

    inputFd = open(inputPath.c_str(), O_RDONLY);
    assert(inputFd != -1);

    if (inputFd == -1)
    {
        LOGE("[slDecoder] Error opening input file");
        return false;
    }

    if (!OpenSLEngine::get()->acquire())
    {
        LOGE("[slDecoder] Error acquiring OpenSL engine for use in the decoder");
        return false;
    }

    // configure audio source (input info):

    sourceLocator.locatorType = SL_DATALOCATOR_ANDROIDFD;
    sourceLocator.fd          = inputFd;
    sourceLocator.length      = SL_DATALOCATOR_ANDROIDFD_USE_FILE_SIZE;
    sourceLocator.offset      = 0;

    sourceFormat.formatType    = SL_DATAFORMAT_MIME;
    sourceFormat.mimeType      = 0;
    sourceFormat.containerType = SL_CONTAINERTYPE_UNSPECIFIED;

    audioSource.pLocator = &sourceLocator;
    audioSource.pFormat  = &sourceFormat;

    // configure audio sink (output info):

    sinkLocator.locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
    sinkLocator.numBuffers = N_BUF;

    // sink format is ignored, but must be filled :/

    sinkFormat.formatType = SL_DATAFORMAT_PCM;
    sinkFormat.samplesPerSec = SL_SAMPLINGRATE_44_1;
    sinkFormat.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
    sinkFormat.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
    sinkFormat.numChannels = 2;
    sinkFormat.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
    sinkFormat.endianness = SL_BYTEORDER_LITTLEENDIAN;

    audioSink.pLocator = &sinkLocator;
    audioSink.pFormat = &sinkFormat;

    static const int N_ITF = 3;
    SLInterfaceID ids[N_ITF] = { SL_IID_ANDROIDSIMPLEBUFFERQUEUE, SL_IID_METADATAEXTRACTION, SL_IID_PREFETCHSTATUS };
    SLboolean required[N_ITF] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
    // NB: SL_IID_PLAY interface is implicit, no need to list it

    try
    {
        SLEngineItf eng = OpenSLEngine::get()->itf();
        res = (*eng)->CreateAudioPlayer(eng, &slPlayer, &audioSource, &audioSink, N_ITF, ids, required);
        slCheckThrow(res,"player creation");

        res = (*slPlayer)->Realize(slPlayer,SL_BOOLEAN_FALSE);
        slCheckThrow(res,"player realization");

        // setup play interface:

        res = (*slPlayer)->GetInterface(slPlayer, SL_IID_PLAY, &slPlay);
        slCheckThrow(res,"play interface retrieval");

        SLuint32 mask = SL_PLAYEVENT_HEADATEND | SL_PLAYEVENT_HEADSTALLED | SL_PLAYEVENT_HEADATNEWPOS;
        res = (*slPlay)->SetCallbackEventsMask(slPlay, mask);
        slCheckThrow(res,"setting position events mask");

        res = (*slPlay)->RegisterCallback(slPlay, OpenSLDecoder::onPositionEvent, this);
        slCheckThrow(res,"registering position callback");

        // setup prefetch (listen for errors in opening the compressed file):

        res = (*slPlayer)->GetInterface(slPlayer,SL_IID_PREFETCHSTATUS,&slPrefetch);
        slCheckThrow(res,"getting prefetch status interface");

        res = (*slPrefetch)->RegisterCallback(slPrefetch, OpenSLDecoder::onPrefetchEvent, this);
        slCheckThrow(res,"prefetch status callback registration");

        res = (*slPrefetch)->SetCallbackEventsMask(slPrefetch, PREFETCH_ERROR_MASK);
        slCheckThrow(res,"prefetch event mask setting");

        // setup metadata extraction:

        res = (*slPlayer)->GetInterface(slPlayer, SL_IID_METADATAEXTRACTION, &slMetadataExtractor);
        slCheckThrow(res,"gettin metadata extractor interface");

        // setup buffer queue:

        res = (*slPlayer)->GetInterface(slPlayer, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &slQueue);
        slCheckThrow(res,"buffer queue interface retrieval");

        res = (*slQueue)->RegisterCallback(slQueue, OpenSLDecoder::onBufferDecoded, this);
        slCheckThrow(res,"buffer queue callback setup");

        for (currentBuffer = 0; currentBuffer<N_BUF; currentBuffer++)
        {
            res = (*slQueue)->Enqueue(slQueue,samplesBuffer[currentBuffer], sizeof(samplesBuffer[currentBuffer]));
            slCheckThrow(res,"initial buffers enqueuing");
        }
        currentBuffer = 0;

        // start prefetching:

        res = (*slPlay)->SetPlayState(slPlay, SL_PLAYSTATE_PAUSED);
        slCheckThrow(res,"pausing for prefetch");

        SLuint32 prefetchStatus = SL_PREFETCHSTATUS_UNDERFLOW;
        SLuint32 timeOutIndex = 25; // time out prefetching after 2.5s
        prefetchError = false;

        while ((prefetchStatus != SL_PREFETCHSTATUS_SUFFICIENTDATA) && (timeOutIndex > 0) && !prefetchError)
        {
            usleep(10 * 1000);
            res = (*slPrefetch)->GetPrefetchStatus(slPrefetch, &prefetchStatus);
            slCheckThrow(res,"getting prefetch status");
            timeOutIndex--;
        }

        if (timeOutIndex == 0 || prefetchError)
        {
            slCheckThrow(SL_RESULT_IO_ERROR,"prefetching audio data");
        }

        res = determineDecodedFormat(slMetadataExtractor,decodedFormat);
        slCheckThrow(res,"determining decoded format");

        duration = getContentDuration(slPlay);

        // start decoding:

        res = (*slPlay)->SetPlayState(slPlay, SL_PLAYSTATE_PLAYING);
        slCheckThrow(res,"activating play");

    }
    catch (OpenSLException& e)
    {
        dispose();
        return false;
    }

    LOGI("[slDecoder] started");
    return true;
}

void OpenSLDecoder::dispose()
{
    LOGI("[slDecoder] dispose");
    if (slPlay)
    {
        SLresult res = (*slPlay)->SetPlayState(slPlay, SL_PLAYSTATE_STOPPED);
        slError(res,"Stopping decoding");
        LOGI("[slDecoder] stopped");
    }

    if (slPlayer)
    {
        (*slPlayer)->Destroy(slPlayer);
        slPlayer = nullptr;
        slPlay = nullptr;
        slQueue = nullptr;
        slMetadataExtractor = nullptr;
        slPrefetch = nullptr;
    }
    LOGI("[slDecoder] release engine");
    OpenSLEngine::get()->release();
    LOGI("[slDecoder] engine released");
}

SLmillisecond OpenSLDecoder::getContentDuration(SLPlayItf playItf)
{
    SLmillisecond durationInMsec = SL_TIME_UNKNOWN;
    SLresult res = (*playItf)->GetDuration(playItf, &durationInMsec);
    if (slError(res,"getting duration"))
        return SL_TIME_UNKNOWN;
    LOGI("[slDecoder] Content duration is %u ms", durationInMsec);
    return durationInMsec;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
void OpenSLDecoder::onBufferDecoded(SLAndroidSimpleBufferQueueItf bq, void* ctx)
{
    OpenSLDecoder* dec = static_cast<OpenSLDecoder*>(ctx);

    if (dec->listener)
    {
        dec->listener->onBufferDecoded(dec, dec->samplesBuffer[dec->currentBuffer], BUFFERSIZE, dec->decodedFormat);
    }

    SLresult res = (*dec->slQueue)->Enqueue(dec->slQueue,
                                            dec->samplesBuffer[dec->currentBuffer],
                                            sizeof(dec->samplesBuffer[dec->currentBuffer]));

    if (slError(res,"re-enqueuing buffer for decoding"))
    {
        if (dec->listener)
            dec->listener->onError(dec);
    }

    dec->currentBuffer++;
    dec->currentBuffer = dec->currentBuffer >= N_BUF ? 0 : dec->currentBuffer;
}

#pragma clang diagnostic pop

void OpenSLDecoder::onPositionEvent(SLPlayItf pitf, void* ctx, SLuint32 event)
{
    OpenSLDecoder* dec = static_cast<OpenSLDecoder*>(ctx);

    if (event & SL_PLAYEVENT_HEADATNEWPOS)
    {
        SLresult res = (*pitf)->GetPosition(pitf, &(dec->progress));

        if (slError(res,"getting position"))
        {
            dec->progress = SL_TIME_UNKNOWN;
        }
    }

    if (event & SL_PLAYEVENT_HEADATEND)
    {
        close(dec->inputFd);

        if (dec->listener)
            dec->listener->onComplete(dec);
    }

    if (event & SL_PLAYEVENT_HEADSTALLED)
    {
        LOGE("[slDecoder] play head stalled");
    }
}

void OpenSLDecoder::onPrefetchEvent( SLPrefetchStatusItf caller,  void *ctx, SLuint32 event)
{
    OpenSLDecoder* dec = static_cast<OpenSLDecoder*>(ctx);
    SLpermille level = 0;
    SLresult result = (*caller)->GetFillLevel(caller, &level);

    if (slError(result,"Get prefetch fill level"))
    {
        dec->prefetchError = true;
        return;
    }

    SLuint32 status;
    result = (*caller)->GetPrefetchStatus(caller, &status);

    if (slError(result,"Getting prefetch status"))
    {
        dec->prefetchError = true;
        return;
    }

    if ((PREFETCH_ERROR_MASK == (event & PREFETCH_ERROR_MASK))
        && (level == 0)
        && (status == SL_PREFETCHSTATUS_UNDERFLOW))
    {
        dec->prefetchError = true;
    }
}

SLresult OpenSLDecoder::determineDecodedFormat(SLMetadataExtractionItf extractor, SLDataFormat_PCM& ret)
{
    SLuint32 mdCount = 0;
    SLuint32 i;
    ret = { SL_DATAFORMAT_PCM, 0, 0, 0, 0, 0, 0 };

    // scan through the metadata items
    SLresult res = (*extractor)->GetItemCount(extractor, &mdCount);

    if (slError(res, "retrieving metadata items count"))
    {
        return res;
    }

    for( i = 0; i < mdCount; ++i )
    {
        SLMetadataInfo *key = NULL;
        SLMetadataInfo *value = NULL;
        SLuint32 itemSize = 0;

        // Get the size of and malloc memory for the metadata item
        (*extractor)->GetKeySize(extractor, i, &itemSize);
        key =  (SLMetadataInfo*) malloc(itemSize);

        if( key )
        {
            // Extract the key into the memory
            res = (*extractor)->GetKey(extractor, i, itemSize, key);

            if (slError(res, "retrieving metadata key"))
            {
                free(key);
                return res;
            }

            res = (*extractor)->GetValueSize(extractor, i, &itemSize);

            if (slError(res, "retrieving metadata value size"))
            {
                free(key);
                return res;
            }

            value = (SLMetadataInfo*) malloc(itemSize);

            if(value)
            {
                // Extract the value into memory
                (*extractor)->GetValue(extractor, i, itemSize, value);

                if (slError(res, "retrieving metadata value size"))
                {
                    free(value);
                    free(key);
                    return res;
                }


                if (strcmp((const char *) key->data, ANDROID_KEY_PCMFORMAT_NUMCHANNELS) == 0)
                {
                    ret.numChannels = *( (SLuint32*) value->data );
                }
                else if( strcmp((const char *)  key->data, ANDROID_KEY_PCMFORMAT_SAMPLERATE ) == 0 )
                {
                    ret.samplesPerSec = *( (SLuint32*)value->data ) * 1000;
                }
                else if( strcmp((const char *)  key->data, ANDROID_KEY_PCMFORMAT_BITSPERSAMPLE ) == 0 )
                {
                    ret.bitsPerSample = *( (SLuint32*)value->data );
                }
                else if( strcmp((const char *)  key->data, ANDROID_KEY_PCMFORMAT_CONTAINERSIZE ) == 0 )
                {
                    ret.containerSize = *( (SLuint32*)value->data );
                }
                else if( strcmp((const char *)  key->data, ANDROID_KEY_PCMFORMAT_CHANNELMASK ) == 0 )
                {
                    ret.channelMask = *( (SLuint32*)value->data );
                }
                else if( strcmp((const char *)  key->data, ANDROID_KEY_PCMFORMAT_ENDIANNESS ) == 0 )
                {
                    ret.endianness = *( (SLuint32*)value->data );
                }
                free(value);
            }
            free(key);
        }
    }

    LOGI("[slDecoder] input metadata. sr: %u, ch: %u, bits: %u",ret.samplesPerSec, ret.numChannels, ret.bitsPerSample);
    return SL_RESULT_SUCCESS;
}

SLDataFormat_PCM OpenSLDecoder::getDecodedFormat() const
{
    return decodedFormat;
}

SLmillisecond OpenSLDecoder::getDuration() const
{
    return duration;
}





