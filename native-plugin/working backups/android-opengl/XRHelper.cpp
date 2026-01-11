#include "XRHelper.h"
#include "Logger.h"
#include <cstring>
#include <string>

namespace XRHelper {
    XrInstance xrInstance;
    XrSession xrSession;
    XrSpace xrAppSpace;
    XrSystemId systemId;

    void SetInstance(uint64_t instance) {
        xrInstance = (XrInstance)instance;
    }

    void SetSession(uint64_t session) {   
        xrSession = (XrSession)session;
    }

    void GetSystem() {
        XrSystemGetInfo sysInfo{XR_TYPE_SYSTEM_GET_INFO};
        sysInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
        xrGetSystem(xrInstance, &sysInfo, &systemId);
    }

    void Loop() {
        XrFrameState frameState{XR_TYPE_FRAME_STATE};
        XrFrameWaitInfo waitInfo{XR_TYPE_FRAME_WAIT_INFO};
        xrWaitFrame(xrSession, &waitInfo, &frameState);

        XrFrameBeginInfo beginInfo{XR_TYPE_FRAME_BEGIN_INFO};
        xrBeginFrame(xrSession, &beginInfo);

        // time warp

        XrFrameEndInfo endInfo{XR_TYPE_FRAME_END_INFO};
        endInfo.displayTime = frameState.predictedDisplayTime;
        endInfo.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
        endInfo.layerCount = 0;
        endInfo.layers = nullptr;
        xrEndFrame(xrSession, &endInfo);
    }

    XrSwapchain CreateSwapchain(int width, int height) {
        Logger::logFormat("xrSession: %p", xrSession);
        Logger::logFormat("Width %d, Height: %d", width, height);
        if (xrSession == nullptr) {
            Logger::logFormat("Invalid xrSession, cannot create swapchain.");
        }

            uint32_t swapchainFormatCount = 0;
    xrEnumerateSwapchainFormats(xrSession, 0, &swapchainFormatCount, nullptr);

    std::vector<int64_t> swapchainFormats(swapchainFormatCount);
    xrEnumerateSwapchainFormats(xrSession, swapchainFormatCount, &swapchainFormatCount, swapchainFormats.data());

    for (uint32_t i = 0; i < swapchainFormatCount; ++i) {
        int64_t fmt = swapchainFormats[i];
        Logger::logFormat("%d", fmt);

        // Optionally print the GL enum name
        switch (fmt) {
            case GL_RGBA8: Logger::log("(GL_RGBA8)"); break;
            case GL_RGB10_A2: Logger::log("(GL_RGB10_A2)"); break;
            case GL_RGBA16F: Logger::log("(GL_RGBA16F)"); break;
            default: Logger::log("(Unknown format)"); break;
        }
    }

        XrSwapchain swapchain;
        XrSwapchainCreateInfo sci{XR_TYPE_SWAPCHAIN_CREATE_INFO};
        sci.next = NULL;
        sci.usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT | XR_SWAPCHAIN_USAGE_SAMPLED_BIT;
        sci.format = 33321;
        sci.width = width;
        sci.height = height;
        sci.faceCount = 1;
        sci.arraySize = 1;
        sci.mipCount = 1;
        sci.sampleCount = 4;

        XrResult res = xrCreateSwapchain(xrSession, &sci, &swapchain);

        if(res != XR_SUCCESS) {
            Logger::logFormat("Failed to create swapchain: %d", res);
        }

        return swapchain;
    }

    std::vector<XrSwapchainImageOpenGLESKHR> QueryImages(XrSwapchain swapchain) {
        uint32_t imageCount = 0;
        XrResult res = xrEnumerateSwapchainImages(swapchain, 0, &imageCount, nullptr);

        if(res != XR_SUCCESS) {
            Logger::logFormat("Faile dto enumerate swapchain images 1: %d", res);
        }

        std::vector<XrSwapchainImageOpenGLESKHR> images(imageCount, { XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_ES_KHR});
        res = xrEnumerateSwapchainImages(swapchain, imageCount, &imageCount, (XrSwapchainImageBaseHeader*)images.data());    
        if(res != XR_SUCCESS) {
            Logger::logFormat("Failed to enumerate swapchain images 2: %d", res);
        }

        for (size_t i = 0; i < images.size(); i++) {
            Logger::logFormat("Swapchain image[%d] texture ID: %u", i, images[i].image);
        }
        
        return images;
    }
}