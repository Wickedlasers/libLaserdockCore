// Android logging
#include <android/log.h>
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG  , "~~~~~~", __VA_ARGS__)
#define DLOG(...) __android_log_print(ANDROID_LOG_DEBUG  , "~~~~~~", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , "~~~~~~", __VA_ARGS__)
#define ELOG(...) __android_log_print(ANDROID_LOG_ERROR  , "~~~~~~", __VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif

void log_dump(const void*addr,int len,int linelen);
void log_dumpf(const char*fmt,const void*addr,int len,int linelen);

#ifdef __cplusplus
}
#endif