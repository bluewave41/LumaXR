#pragma once
#define XR_USE_GRAPHICS_API_OPENGL_ES
#define XR_USE_PLATFORM_ANDROID
#include <jni.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include "openxr/openxr.h"
#include "openxr/openxr_platform.h"
#include <vector>
#include <EGL/egl.h>

namespace XRHelper {
    XrSwapchain CreateSwapchain(int width, int height);
    std::vector<XrSwapchainImageOpenGLESKHR> QueryImages(XrSwapchain swapchain);
    void SetInstance(uint64_t instance);
    void SetSession(uint64_t session);
}