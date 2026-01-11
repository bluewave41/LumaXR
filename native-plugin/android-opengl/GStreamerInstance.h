#pragma once

#include <cstring>
#include <android/native_window.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <gst/gst.h>
#include <string>
#include <vector>
#include <gst/gl/egl/gstgldisplay_egl.h>
#include "jni.h"

class GStreamerInstance {
  public:
    int id;
    int port;
    std::string pipelineString;
    GstElement *pipeline;
    int width;
    int height;
    ANativeWindow *nativeWindow;
    jobject surface;

    GStreamerInstance(int _width, int _height, int _port, std::string _pipelineString);
    ~GStreamerInstance();
};

extern std::vector<GStreamerInstance*> gstreamerInstances;

GStreamerInstance* GetGStreamerInstance(int instanceId);