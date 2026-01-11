#pragma once
#include "IUnityLog.h"
#include <android/log.h>

namespace Logger {
    void init(IUnityLog *unityLogPTr);
    void log(const char *message);
    void logFormat(const char *fmt, ...);
}