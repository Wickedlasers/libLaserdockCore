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
 * requested that these non-binding requests be included aint with the 
 * license above.
 */

/**
 * \file AudioDecoderOpenSL.h
 * \class AudioDecoderOpenSL
 */

#ifndef AUDIODECODEROPENSL_H
#define AUDIODECODEROPENSL_H

#include <memory>

#include "audiodecoderbase.h"

#include "../src/android/OpenSLDecoder.hpp"

class AudioDecoderOpenSL : public AudioDecoderBase, public OpenSLDecoderListener {
public:
    AudioDecoderOpenSL(const std::string filename);
    ~AudioDecoderOpenSL();

    void initAndroidEnv(JNIEnv* env, jobject a = nullptr);

    // Overriding AudioDecoderBase 
    int open();
    int seek(int sampleIdx);
    int read(int size, const SAMPLE *buffer);
    static std::vector<std::string> supportedFileExtensions();

    void onBufferDecoded(OpenSLDecoder* d, short* buf, int nSamples, SLDataFormat_PCM& format) override;
    void onComplete(OpenSLDecoder *d) override;
    void onError(OpenSLDecoder *d) override;

private:
    std::unique_ptr<OpenSLDecoder> m_decoder;
    jobject activity = nullptr;
    double startTime;
    JavaVM* vm;
    JNIEnv* uiEnv;

    void callbackActivity(JNIEnv *env, bool result, double duration);
    JNIEnv *attachToVm();

    std::vector<short> m_buffer;
};


#endif // ifndef AUDIODECODEROPENSL_H
