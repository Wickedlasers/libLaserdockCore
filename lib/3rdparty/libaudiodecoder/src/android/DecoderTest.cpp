#include <jni.h>
#include <string>
#include <chrono>
#include "OpenSLDecoder.hpp"
#include "AndroidLogging.h"

double getTime()
{
    auto now = std::chrono::high_resolution_clock::now();
    double nowSecs = 1e-9 * now.time_since_epoch().count();
    return nowSecs;
}

class NativeContext : public OpenSLDecoderListener {

    jobject activity;
    OpenSLDecoder decoder;
    double startTime;
    JavaVM* vm;
    JNIEnv* uiEnv;

public:

    NativeContext(JNIEnv* env, jobject a, std::string file)
    {
        uiEnv = env;
        uiEnv->GetJavaVM(&vm);
        activity = uiEnv->NewGlobalRef(a);

        decoder.setInput(file);
        decoder.setListener(this);
    }

    ~NativeContext()
    {
        decoder.dispose();
    }

    bool startDecoding()
    {
        startTime = getTime();
        return decoder.start();
    }

    void onBufferDecoded(OpenSLDecoder* d, short* buf, int nSamples, SLDataFormat_PCM& format) override
    {
        // here you can write to file
    }

    void onComplete(OpenSLDecoder* d) override
    {
        JNIEnv* env = attachToVm();
        double duration = getTime() - startTime;
        callbackActivity(env,true,duration);
        env->DeleteGlobalRef(activity);
        vm->DetachCurrentThread();
    }

    virtual void onError(OpenSLDecoder* d)
    {
        JNIEnv* env = attachToVm();
        callbackActivity(env,false,-1.0);
        env->DeleteGlobalRef(activity);
        vm->DetachCurrentThread();
    }

    void callbackActivity(JNIEnv* env, bool result, double duration)
    {
        jclass cls = env->GetObjectClass(activity);
        jmethodID mid = env->GetMethodID(cls,"onFileDecoded","(ZD)V");
        env->CallVoidMethod(activity,mid,true,(jdouble)duration);
    }

    JNIEnv* attachToVm()
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
};


extern "C" JNIEXPORT jlong JNICALL
Java_test_decoding_audio_TestActivity_decodeWithOpenSL(JNIEnv *env, jobject activity, jstring file_)
{
    const char *file = env->GetStringUTFChars(file_, 0);
    NativeContext *ctx = new NativeContext(env, activity, file);

    bool res = ctx->startDecoding();

    if (!res)
    {
        delete ctx;
        ctx = nullptr;
    }

    env->ReleaseStringUTFChars(file_, file);
    return (jlong)ctx;
}

extern "C" JNIEXPORT void JNICALL
Java_test_decoding_audio_TestActivity_destroyNativeContext(JNIEnv *env, jobject instance, jlong nativePtr)
{
    NativeContext* ctx = reinterpret_cast<NativeContext*>(nativePtr);
    delete ctx;
}

