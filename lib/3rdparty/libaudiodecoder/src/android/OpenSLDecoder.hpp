#ifndef ANDROID_OPENSLDECODER_HPP
#define ANDROID_OPENSLDECODER_HPP

#include "OpenSLEngine.hpp"

class OpenSLDecoder;
class OpenSLDecoderListener {
public:
    virtual void onBufferDecoded(OpenSLDecoder* d, short* buf, int nSamples, SLDataFormat_PCM& format) = 0;
    virtual void onComplete(OpenSLDecoder* decoder) = 0;
    virtual void onError(OpenSLDecoder* decoder) = 0;
};

class OpenSLDecoder {

public:

    void setInput(std::string path);
    void setListener(OpenSLDecoderListener* listener);
    bool start();
    float getProgress();
    void dispose();

    static void onBufferDecoded(SLAndroidSimpleBufferQueueItf bq, void* ctx);
    static void onPositionEvent(SLPlayItf pitf, void* ctx, SLuint32 event);
    static void onPrefetchEvent(SLPrefetchStatusItf caller, void *ctx, SLuint32 event);

    static SLmillisecond getContentDuration(SLPlayItf playItf);
    static SLresult determineDecodedFormat(SLMetadataExtractionItf extractor, SLDataFormat_PCM& ret);

    SLDataFormat_PCM getDecodedFormat() const;
    SLmillisecond getDuration() const;

private:

    static const int BUFFERSIZE = 1024;
    static const int N_BUF = 2;
    short samplesBuffer[N_BUF][BUFFERSIZE];
    int currentBuffer = 0;

    int inputFd;
    std::string inputPath;

    OpenSLDecoderListener* listener = nullptr;

    SLObjectItf  slPlayer = nullptr;
    SLPlayItf slPlay = nullptr;
    SLAndroidSimpleBufferQueueItf slQueue = nullptr;
    SLMetadataExtractionItf slMetadataExtractor = nullptr;
    SLPrefetchStatusItf slPrefetch = nullptr;

    SLDataLocator_AndroidFD sourceLocator;
    SLDataFormat_MIME sourceFormat;
    SLDataSource audioSource;

    SLDataLocator_AndroidSimpleBufferQueue sinkLocator;
    SLDataFormat_PCM sinkFormat;
    SLDataSink audioSink;

    SLDataFormat_PCM decodedFormat;

    bool prefetchError = false;
    SLmillisecond duration = SL_TIME_UNKNOWN;
    SLmillisecond progress = SL_TIME_UNKNOWN;
};


#endif // ANDROID_OPENSLDECODER_HPP
