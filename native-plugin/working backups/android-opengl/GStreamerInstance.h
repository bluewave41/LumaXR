#pragma once

#include "XRHelper.h"
#include <cstring>
#include <android/native_window.h>
#include <android/surface_texture.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <gst/gst.h>
#include <string>
#include <vector>
#include <algorithm>
#include <gst/gl/egl/gstgldisplay_egl.h>

class GStreamerInstance {
  public:
    int id;
    int port;
    std::string pipelineString;
    GstElement *pipeline;
    int width;
    int height;
    GLuint textureId;
    GLuint oesTextureId;
    GstSample *sample;
    pthread_mutex_t mutex;
    ANativeWindow *nativeWindow;
    ASurfaceTexture *surfaceTexture;
    GLuint fbo;
    uint samples;
    double lastTime;
    XrSwapchain swapchain;
    std::vector<XrSwapchainImageOpenGLESKHR> swapchainImages;

    GStreamerInstance(int _width, int _height, int _port, std::string _pipelineString);
};

extern std::vector<GStreamerInstance*> gstreamerInstances;

GStreamerInstance* GetGStreamerInstance(int instanceId);