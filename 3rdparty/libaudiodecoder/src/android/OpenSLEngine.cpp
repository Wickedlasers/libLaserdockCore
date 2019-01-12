#include "OpenSLEngine.hpp"
#include "AndroidLogging.h"
#include <cassert>
#include <sstream>

SLuint32 toOpenSLSampleRate(int sr)
{
    return sr * 1000;
}


std::string slResultToString(SLresult res)
{
    switch (res)
    {
        case SL_RESULT_SUCCESS: return "Success";
        case SL_RESULT_BUFFER_INSUFFICIENT: return "Buffer insufficient";
        case SL_RESULT_CONTENT_CORRUPTED: return "Content corrupted";
        case SL_RESULT_CONTENT_NOT_FOUND: return "Content not found";
        case SL_RESULT_CONTENT_UNSUPPORTED: return "Content unsupported";
        case SL_RESULT_CONTROL_LOST: return "Control lost";
        case SL_RESULT_FEATURE_UNSUPPORTED: return "Feature unsupported";
        case SL_RESULT_INTERNAL_ERROR: return "Internal error";
        case SL_RESULT_IO_ERROR: return "IO error";
        case SL_RESULT_MEMORY_FAILURE: return "Memory failure";
        case SL_RESULT_OPERATION_ABORTED: return "Operation aborted";
        case SL_RESULT_PARAMETER_INVALID: return "Parameter invalid";
        case SL_RESULT_PERMISSION_DENIED: return "Permission denied";
        case SL_RESULT_PRECONDITIONS_VIOLATED: return "Preconditions violated";
        case SL_RESULT_RESOURCE_ERROR: return "Resource error";
        case SL_RESULT_RESOURCE_LOST: return "Resource lost";
        case SL_RESULT_UNKNOWN_ERROR: return "Unknown error";
        default: return "Undefined error";
    }
}


bool slError(SLresult res, std::string msg)
{
    if (res!=SL_RESULT_SUCCESS)
        LOGE("[OpenSL] %s - Error: %s",msg.c_str(),slResultToString(res).c_str());
    return res != SL_RESULT_SUCCESS;
}


void slCheckThrow(SLresult res, std::string msg)
{
    if (res!=SL_RESULT_SUCCESS)
    {
        std::stringstream ss;
        ss << "[OpenSL] " << msg << " - Exception: " << slResultToString(res);
        LOGE("%s",ss.str().c_str());
        throw OpenSLException(ss.str());
    }
}


bool slCheckAssert(SLresult res,std::string msg)
{
    if (res!=SL_RESULT_SUCCESS)
        LOGE("[OpenSL] %s - Fatal error: %s",msg.c_str(),slResultToString(res).c_str());
    assert(res==SL_RESULT_SUCCESS);
    return res!=SL_RESULT_SUCCESS;
}


/*
 *  Engine definitions
 */


OpenSLEngine* OpenSLEngine::instance = nullptr;


OpenSLEngine *OpenSLEngine::get()
{
    if (instance==nullptr)
    {
        instance = new OpenSLEngine();
    }
    return instance;
}


SLEngineItf OpenSLEngine::itf()
{
    assert(slEngine);
    return slEngine;
}


bool OpenSLEngine::init()
{
    SLresult res;

    if (nullptr == slEngineObj)
    {
        res = slCreateEngine(&slEngineObj,0,nullptr,0,nullptr,nullptr);

        if (slCheckAssert(res,"creating opensl engine"))
            return false;

        res = (*slEngineObj)->Realize(slEngineObj,SL_BOOLEAN_FALSE);

        if (slCheckAssert(res,"realizing opensl engine"))
            return false;
    }

    if (nullptr==slEngine)
    {
        res = (*slEngineObj)->GetInterface(slEngineObj, SL_IID_ENGINE, &slEngine);
        return !slCheckAssert(res,"getting opensl engine interface");
    }

    return true;
}


bool OpenSLEngine::acquire()
{
    if (referenceCount == 0 )
    {
        bool inited = init();
        referenceCount += inited ? 1 : 0;
        return inited;
    }
    else
    {
        referenceCount++;
        return true;
    }
}

bool OpenSLEngine::release()
{
    assert(referenceCount > 0);
    referenceCount -= referenceCount > 0 ? 1 : 0;

    if (referenceCount == 0)
    {
        destroy();
        return true;
    }

    return false;
}

void OpenSLEngine::destroy()
{
    if (slEngineObj)
    {
        (*slEngineObj)->Destroy(slEngineObj);
        slEngineObj = nullptr;
        slEngine = nullptr;
    }
}