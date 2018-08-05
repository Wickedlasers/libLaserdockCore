#ifndef ANDROID_OPENSLENGINE_HPP
#define ANDROID_OPENSLENGINE_HPP

#include <string>
#include <stdexcept>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>


SLuint32 toOpenSLSampleRate(int sr);
std::string slResultToString(SLresult res);

bool slError(SLresult res, std::string msg);
bool slCheckAssert(SLresult res, std::string msg);
void slCheckThrow(SLresult res, std::string msg);


class OpenSLException : public std::runtime_error { using std::runtime_error::runtime_error; };

class OpenSLEngine {

public:

    /**
     * Retrieve the engine singleton.
     * (There can be one openSL engine for each app)
     */

    static OpenSLEngine* get();

    bool acquire();
    bool release();

    SLEngineItf itf();

private:

    static OpenSLEngine* instance;

    OpenSLEngine() {}

    unsigned int referenceCount = 0;

    SLObjectItf slEngineObj = nullptr;
    SLEngineItf slEngine    = nullptr;

    bool init();
    void destroy();

};

#endif // ANDROID_OPENSLENGINE_HPP
