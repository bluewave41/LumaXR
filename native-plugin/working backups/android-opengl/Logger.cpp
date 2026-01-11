#include "IUnityLog.h"
#include <android/log.h>

namespace Logger {
    static IUnityLog *g_unityLogPtr;

    void init(IUnityLog *unityLogPtr) {
        g_unityLogPtr = unityLogPtr;
    }

    void log(const char *message) {
        __android_log_print(ANDROID_LOG_INFO, "LumaXR", message);
    }

    void logFormat(const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);

        __android_log_vprint(ANDROID_LOG_INFO, "LumaXR", fmt, args);

        va_end(args);
    }
}