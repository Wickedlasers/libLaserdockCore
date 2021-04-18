/**
    libLaserdockCore
    Copyright(c) 2018 Wicked Lasers

    This file is part of libLaserdockCore.

    libLaserdockCore is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libLaserdockCore is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libLaserdockCore.  If not, see <https://www.gnu.org/licenses/>.
**/

#include <ldCore/Sound/ldSoundAnalyzer.h>

/*!
  \class ldSoundAnalyzer
  \brief Analyzer for an audio source
  \inmodule audio

  This class is used to analyze a segment of audio data and extract relevant data from it.
  This data can then be used to generate live visualization.
*/

#include <QtCore/QDebug>
#include <QtCore/QTimer>

#include <ldCore/Sound/ldSoundInterface.h>

namespace  {
const int MAX_LAG_BUFFER_MS = 1000;
const int MAX_SOUND_UPDATE_FRAMES = 4*SAMPLE_SIZE; // sample_size is measured in frames
}

float ldSoundAnalyzer::s_volumeCoeff = 1;


ldSoundAnalyzer::ldSoundAnalyzer(QObject *parent) :
    QObject(parent)
{
    // init audio buffer.  allocate 4 audio blocks worth of storage
    int maxBufferSize = 4 * AUDIO_BLOCK_SIZE;
    int maxLagSize = 44.100 * MAX_LAG_BUFFER_MS * 2; // max lag

    m_pAudioBuffer.reset(new ldThreadSafeQueue<float>(maxBufferSize + maxLagSize));

    // init and zero out our persistent audio block
    m_pCurrentBlock.reset(new AudioBlock);
    memset(m_pCurrentBlock.get(), 0, sizeof(AudioBlock));
}

ldSoundAnalyzer::~ldSoundAnalyzer()
{
}

void ldSoundAnalyzer::handleSoundUpdated(const char * data, qint64 len) {

    auto soundInterface = qobject_cast<ldSoundInterface*>(sender());
    // if signal was disconnected we shouldn't process
    if(!soundInterface) {
        return;
    }
    // get audio format variable
    const QAudioFormat &format = soundInterface->getAudioFormat();

    // prevent invalid events from crash later in division by zero for format.bytesPerFrame()
    if(!format.isValid()) {
        qWarning() << "Sound update: Invalid format" << sender() << len;
        return;
    }

    // temp buffer for stereo floats
    // maximum size of input data is 4 blocks.
    std::vector<float> convertedBuffer; // 2 floats per frame (stereo)
    convertedBuffer.resize(2*MAX_SOUND_UPDATE_FRAMES);

    //qDebug() << format.bytesPerFrame();

    // calculate number of frames
    int frames = (int)(len / format.bytesPerFrame());

    // if too many frames are in this update, reduce the size of the len variable.
    if (frames > MAX_SOUND_UPDATE_FRAMES) {
        frames = MAX_SOUND_UPDATE_FRAMES;
        qDebug() << "Warning, recv'd " << frames << " frames which is larger than max of " << MAX_SOUND_UPDATE_FRAMES;
    }

    // convert samples and send to the locked buffer
    convertRawToStereoFloatFrame(data, frames, format, convertedBuffer.data());

    processAudioBuffer(convertedBuffer.data(), frames, format.sampleRate());
}

// convert samples from raw format to floating point stereo interleaved
// TODO: implement other types
void ldSoundAnalyzer::convertRawToStereoFloatFrame(const char *data, qint64 frames, const QAudioFormat &m_format, float interleavedFrame[]) {

    if (frames == 0) return;

    // only format supported now
    bool isAndroidVisualizerFormat = (m_format.sampleSize() == 8)
                                     && (m_format.sampleType() == QAudioFormat::SignedInt || m_format.sampleType() == QAudioFormat::UnSignedInt);
    bool isMicFormat = (m_format.sampleSize() == 16 || m_format.sampleSize() == 24 || m_format.sampleSize() == 32)
                       && (m_format.sampleType() == QAudioFormat::SignedInt); // only signed int supported now
    bool isFloatFormat = m_format.sampleType() == QAudioFormat::Float
                         && (m_format.sampleSize() == 32) // only 32-bit supported
                         && m_format.byteOrder() == QAudioFormat::LittleEndian; // only little endian is supported now
    if (!(isAndroidVisualizerFormat || isMicFormat || isFloatFormat)) {
        qWarning() << "warning: audio format not supported: " << m_format.sampleSize() << " bit " << m_format.sampleType();
        return;
    }

    // check for data
    if(!data)
        return;

    QAudioFormat::SampleType sampleType = m_format.sampleType();
    int sampleSize = m_format.sampleSize();
    int bytesPerFrame = m_format.bytesPerFrame();
    int channelCount = m_format.channelCount();
    QAudioFormat::Endian byteOrder = m_format.byteOrder();
    int bytesPerSample = bytesPerFrame/channelCount;

    // temp buffer for max 32 bits (4 bytes)
    qint8 t[4];

    // for each frame in data
    for (int i = 0; i < frames; ++i) {
        int frameOffset = i*bytesPerFrame;

        // for each sample in frame
        for (int channel = 0; channel < channelCount; ++channel) {
            int currentOffset = frameOffset + channel*bytesPerSample;
            float fvalue = 0.f;

            if(isAndroidVisualizerFormat) {
                if(sampleType == QAudioFormat::SignedInt) {
                    qint8 byte = data[currentOffset];
                    fvalue = byte / 128.f;
                } else {
                    quint8 byte = static_cast<quint8>(data[currentOffset]);
                    fvalue = byte / 256.f;
                }
            } else if(isMicFormat){
                // pointer to first byte in sample
                qint8 *ptr = (qint8*) (data + currentOffset);

                // if it's big endian, copy bytes to temp buffer before and set correct byte position
                if (byteOrder == QAudioFormat::BigEndian) {
                    if(sampleSize == 16) {
                        t[0] = ptr[1];
                        t[1] = ptr[0];
                    } else if(sampleSize == 24) {
                        t[0] = ptr[2];
                        t[1] = ptr[1];
                        t[2] = ptr[0];
                    } else if(sampleSize == 32) {
                        t[0] = ptr[3];
                        t[1] = ptr[2];
                        t[2] = ptr[1];
                        t[3] = ptr[0];
                    }

                    ptr = &t[0];
                }

                if(sampleSize == 16) {
                    fvalue = *((qint16*)(ptr));
                    // scale output
                    fvalue /= 32768.0f;
                } else if(sampleSize == 24) {
                    fvalue = ((*((qint32*)(ptr))) << 8);
                    // scale output
                    fvalue /= 2147483648.0f;
                } else if(sampleSize == 32) {
                    fvalue = (*((qint32*)(ptr)));
                    // scale output
                    fvalue /= 2147483648.0f;
                }
            } else if(isFloatFormat) {
                const qint32 *ptr = (const qint32*) (data + currentOffset);
                fvalue = *((float*)(ptr));
            }

            fvalue *= s_volumeCoeff;
            // clamp to [-1:1]
            if (fvalue < -1) fvalue = -1;
            if (fvalue >  1) fvalue =  1;

            // assign left and/or right values based on stereo/mono
            if (channel == 0) {
                interleavedFrame[i*2] = fvalue;
            }
            else if (channel == 1) {
                interleavedFrame[i*2+1] = fvalue;
            }
        }

        // if it is mono set both channels equal
        if(channelCount == 1) {
            interleavedFrame[i*2+1] = interleavedFrame[i*2];
        }
    }
}


void ldSoundAnalyzer::processAudioBuffer(float *convertedBuffer, int frames, int sampleRate)
{
    // find resampling ratio
    float ratio = 1;
    if (sampleRate >= 8000 && sampleRate != 44100) {
        ratio = 44100.f/sampleRate;
    }

    if (ratio == 1) {
        // dont need to resample, enqueue buffer
        m_pAudioBuffer->enQueue(convertedBuffer, 2*frames); // 2 floats per frame (stereo)
    } else {
        // resample then enqueue
        float resampledBuffer[2*MAX_SOUND_UPDATE_FRAMES];
        int oldframes = frames;
        frames = oldframes * ratio;
        if (frames > MAX_SOUND_UPDATE_FRAMES) {
            frames = MAX_SOUND_UPDATE_FRAMES;
            qDebug() << "Warning, recv'd " << frames << " resampled frames which is larger than max of " << MAX_SOUND_UPDATE_FRAMES;
        }
        for (int i = 0; i < frames; i++) {
            int j = i / ratio;
            resampledBuffer[i*2+0] = convertedBuffer[j*2+0];
            resampledBuffer[i*2+1] = convertedBuffer[j*2+1];
        }
        m_pAudioBuffer->enQueue(resampledBuffer, 2*frames); // 2 floats per frame (stereo)
    }

    // done using audio buffer, so we can exit this thread
    // next we have to send audio to the visualizer
    // continue work in the "sendBlocks" method

    // we can use a direct call or a queued connection
    // use queued connection if we were called from an unsafe thread
    // this is because we need visualizer update and draw to get
    // called from the same thread.

    // if the audio device implementation already calls us
    // from the correct thread, we can disable queued call
    // for improved performance

    //    bool useQueuedCall = false; // disable for speed
//    bool useQueuedCall = true; // enable for safety

//    if (useQueuedCall) {
        //        qDebug() << m_pAudioBuffer->getSize();
        // queued call
        if (m_pAudioBuffer->getSize() >= AUDIO_SUB_BLOCK_SIZE) // only if necessary
            QTimer::singleShot(0, this, &ldSoundAnalyzer::sendBlocks);
//    } else {
        // direct call.
//        sendBlocks();
//    }
}

void ldSoundAnalyzer::sendBlocks() {
    // process up to maxBlocks frames of audio at once
    int maxBlocks = 8;// * AUDIO_OVERDRIVE_FACTOR;

    // process blocks
    for (int bidx = 0; bidx < maxBlocks; bidx++) {

        // if >= 1 full frame available, then proces
        if (m_pAudioBuffer->getSize() >= AUDIO_SUB_BLOCK_SIZE) {

            // prepare to get a sub-block of data
            AudioBlock newSubBlock;

            // dequeue 1 sub-block worth of data
            m_pAudioBuffer->deQueue(newSubBlock.data, AUDIO_SUB_BLOCK_SIZE);

            // make room for new audio data in our persistent buffer
            // shift m_pCurrentBlock's data by 1 sub-block
            const int samplesToKeep = AUDIO_BLOCK_SIZE - AUDIO_SUB_BLOCK_SIZE;
            for (int i = 0; i < samplesToKeep; i++)
                m_pCurrentBlock->data[i] = m_pCurrentBlock->data[i+AUDIO_SUB_BLOCK_SIZE];

            // add new data into the buffer
            // copy sub-block to the end of current block
            for (int i = 0; i < AUDIO_SUB_BLOCK_SIZE; i++)
                m_pCurrentBlock->data[i+samplesToKeep] = newSubBlock.data[i];

            // old memcyp implementations
            //memcpy(m_pCurrentBlock->data, m_pCurrentBlock->data+AUDIO_SUB_BLOCK_SIZE, sizeof(float)*(AUDIO_SUB_BLOCK_SIZE-AUDIO_SUB_BLOCK_SIZE));
            //memcpy(m_pCurrentBlock->data+(AUDIO_BLOCK_SIZE-AUDIO_SUB_BLOCK_SIZE), newSubBlock.data, sizeof(float)*AUDIO_SUB_BLOCK_SIZE);

            // block is ready.  send via signal/slots to visualization task
            emit audioBlockUpdated(*m_pCurrentBlock);
        }
    }

}

