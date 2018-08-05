#ifndef ANDROID_ANDROID_LOGGING_H
#define ANDROID_ANDROID_LOGGING_H

#if __ANDROID__

#include <android/log.h>

#define  LOG_TAG    "DecodeTest"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// NB: LOGI and LOGE will log in release builds as well. Use them with parsimony!

#ifdef NDEBUG // release build -> no ops

#define  LOGW(...)  (void)0
#define  LOGD(...)  (void)0
#define  LOGV(...)  (void)0

#else

#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define  LOGV(...)  __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#endif

#else // no-ops for other platforms

#define  LOGI(...) (void)0
#define  LOGE(...) (void)0
#define  LOGW(...) (void)0
#define  LOGD(...) (void)0
#define  LOGV(...) (void)0

#endif

#endif //ANDROID_ANDROID_LOGGING_H
