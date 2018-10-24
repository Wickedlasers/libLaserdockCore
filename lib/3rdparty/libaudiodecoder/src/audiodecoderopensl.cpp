/*
 * libaudiodecoder - Native Portable Audio Decoder Library
 * libaudiodecoder API Header File
 * Latest version available at: http://www.oscillicious.com/libaudiodecoder
 *
 * Copyright (c) 2010-2012 Albert Santoni, Bill Good, RJ Ryan
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * The text above constitutes the entire libaudiodecoder license; however,
 * the Oscillicious community also makes the following non-binding requests:
 *
 * Any person wishing to distribute modifications to the Software is
 * requested to send the modifications to the original developer so that
 * they can be incorporated into the canonical version. It is also
 * requested that these non-binding requests be included along with the
 * license above.
 */

#include <string>
#include <iostream>
#include <jni.h>
#include <chrono>

#include <QtCore/QtDebug>

#include "android/AndroidLogging.h"
#include "../include/audiodecoderopensl.h"

double getTime()
{
    auto now = std::chrono::high_resolution_clock::now();
    double nowSecs = 1e-9 * now.time_since_epoch().count();
    return nowSecs;
}

AudioDecoderOpenSL::AudioDecoderOpenSL(const std::string filename)
    : AudioDecoderBase(filename)
    , m_decoder(new OpenSLDecoder)
{
    m_decoder->setInput(filename);
    m_decoder->setListener(this);
}

AudioDecoderOpenSL::~AudioDecoderOpenSL() {
    m_decoder->dispose();
}

void AudioDecoderOpenSL::initAndroidEnv(JNIEnv *env, jobject a)
{
    uiEnv = env;
    uiEnv->GetJavaVM(&vm);

    if(a) activity = uiEnv->NewGlobalRef(a);
}

int AudioDecoderOpenSL::open() {
    m_buffer.clear();

    if(m_decoder->start()) {
        SLDataFormat_PCM decodedFormat = m_decoder->getDecodedFormat();
        SLmillisecond durationMs = m_decoder->getDuration();
        SLuint32 samplesPerMs = decodedFormat.samplesPerSec / 1000;
        m_iChannels = decodedFormat.numChannels;
        m_iSampleRate = samplesPerMs;
        m_fDuration = durationMs / 1000.f;
        m_iPositionInSamples = 0;

        // calculate number of samples based on other data
        m_iNumSamples = m_fDuration * samplesPerMs * m_iChannels;

//        qDebug() << __FUNCTION__ << m_iNumSamples << m_iChannels << m_iSampleRate << m_fDuration << m_iPositionInSamples;
        return AUDIODECODER_OK;
    } else {
        return AUDIODECODER_ERROR;
    }
}

int AudioDecoderOpenSL::seek(int sampleIdx) {
    m_iPositionInSamples = sampleIdx;
    return m_iPositionInSamples; //filepos;
}

int AudioDecoderOpenSL::read(int size, const SAMPLE *destination) {
    // check for start range
    if(m_iPositionInSamples > m_buffer.size()) {
        qWarning() << __FUNCTION__ << "buffer not filled yet" << m_iPositionInSamples << m_buffer.size();
        return 0;
    }

    // check for end range
    int endPos = m_iPositionInSamples + size;
    if(endPos > m_buffer.size()) {
        qDebug() << __FUNCTION__ << "reduce buffer size" << m_iPositionInSamples << m_buffer.size();
        size = m_buffer.size() - m_iPositionInSamples;
    }

    // fill dest buffers
    SAMPLE *destBufferFloat(const_cast<SAMPLE*>(destination));
    // convert short to float
    const int sampleMax = 1 << (m_decoder->getDecodedFormat().bitsPerSample-1);
    for (long i = 0; i < size; i++)
        destBufferFloat[i] = m_buffer[m_iPositionInSamples + i] / (float) sampleMax;

    m_iPositionInSamples += size;

    return size;
}

// static
std::vector<std::string> AudioDecoderOpenSL::supportedFileExtensions() {
    std::vector<std::string> list;
    // check what opensl support, we need mp3 only for now
    list.push_back(std::string("mp3"));
    return list;
}

void AudioDecoderOpenSL::onBufferDecoded(OpenSLDecoder *d, short *buf, int nSamples, SLDataFormat_PCM &format)
{
    for(int i = 0; i < nSamples; i++)
        m_buffer.push_back(buf[i]);
}

void AudioDecoderOpenSL::onComplete(OpenSLDecoder* d)
{
    JNIEnv* env = attachToVm();
    double duration = getTime() - startTime;
    callbackActivity(env,true,duration);
    if(activity) env->DeleteGlobalRef(activity);
    vm->DetachCurrentThread();
}

void AudioDecoderOpenSL::onError(OpenSLDecoder* d)
{
    JNIEnv* env = attachToVm();
    callbackActivity(env,false,-1.0);
    if(activity) env->DeleteGlobalRef(activity);
    vm->DetachCurrentThread();
}

void AudioDecoderOpenSL::callbackActivity(JNIEnv* env, bool result, double duration)
{
    if(!activity)
        return;

    jclass cls = env->GetObjectClass(activity);
    jmethodID mid = env->GetMethodID(cls,"onFileDecoded","(ZD)V");
    env->CallVoidMethod(activity,mid,true,(jdouble)duration);
}

JNIEnv* AudioDecoderOpenSL::attachToVm()
{
    JNIEnv* env;
    jint res = vm->GetEnv((void**) &env, JNI_VERSION_1_6);

    if (res == JNI_EDETACHED)
    {
        res = vm->AttachCurrentThread(&env, nullptr);
        if (res != JNI_OK)
            LOGE("Can't attach current thread to jvm!");
    }
    return env;
}
