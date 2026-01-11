#include "JNIUtils.h"

namespace JNIUtils {
    JavaVM* g_vm = nullptr;

    JNIEnv* Attach() {
        if(!g_vm) {
            Logger::log("Java VM is null.");
            return nullptr;
        }

        JNIEnv* env = nullptr;
        int result = g_vm->AttachCurrentThread(&env, nullptr);
        if (result != JNI_OK || !env) {
            Logger::log("ERROR: Failed to attach to current thread");
            return nullptr;
        }
        return env;
    }

    void Detatch() {
        g_vm->DetachCurrentThread();
    }
}