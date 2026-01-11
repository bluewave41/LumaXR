#pragma once
#include <gst/gst.h>
#include <gst/gl/gl.h>
#include "Logger.h"
#include "GStreamerInstance.h"


namespace GstUtils {
    void print_negotiated_caps(GstElement *pipeline);
    void print_context(GStreamerInstance *instance);
    void print_element_caps(GstElement* element);
    void print_decoders();
}