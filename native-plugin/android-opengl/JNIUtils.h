#pragma once 

#include <jni.h>
#include "Logger.h"

namespace JNIUtils {
    extern JavaVM *g_vm;

    JNIEnv* Attach();
    void Detatch();
}