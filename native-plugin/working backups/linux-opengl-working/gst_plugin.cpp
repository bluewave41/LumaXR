#include <gst/gst.h>
#include <gst/gl/gl.h>
#include <EGL/egl.h>
#include <gst/gl/gstglcontext.h>
#include <gst/app/gstappsink.h>
#include <cstdint>
#include <string>
#include <cstring>
#include "IUnityInterface.h"
#include "IUnityGraphics.h"
#include "IUnityLog.h"
#include <dlfcn.h>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <GL/glew.h>
#include <gst/gl/egl/gstgldisplay_egl.h>
#include <gst/gl/gstgldisplay.h>
#include <gst/gl/gstglapi.h>
#include <GL/glx.h>

inline std::string format_with_location(const char* file, int line, const char* fmt, ...) {
    char buffer[512];
    va_list args;
    va_start(args, fmt);
    std::vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    return std::string("[") + file + ":" + std::to_string(line) + "] " + buffer;
}

#define FORMAT_ARGS(fmt, ...) format_with_location(__FILE__, __LINE__, fmt, ##__VA_ARGS__).c_str()
#define DEBUG_LOG_FORMAT(PTR, FMT, ...) UNITY_LOG(PTR, FORMAT_ARGS(FMT, ##__VA_ARGS__))

#define FORMAT(MESSAGE) std::string("[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "] " + MESSAGE).c_str()
#define DEBUG_LOG(PTR, MESSAGE) UNITY_LOG(PTR, FORMAT(MESSAGE))
#define DEBUG_LOG_FORMAT(PTR, FMT, ...) UNITY_LOG(PTR, FORMAT_ARGS(FMT, ##__VA_ARGS__))
#define DEBUG_WARNING(PTR, MESSAGE) UNITY_LOG_WARNING(PTR, FORMAT(MESSAGE))
#define DEBUG_ERROR(PTR, MESSAGE) UNITY_LOG_ERROR(PTR, FORMAT(MESSAGE))

static IUnityLog* unityLogPtr = nullptr;

static IUnityInterfaces *s_UnityInterfaces = nullptr;
static IUnityGraphics *s_Graphics = nullptr;
static GstGLContext *glContext = nullptr;
static UnityGfxRenderer s_RendererType = kUnityGfxRendererNull;

static int id = 1000;

struct GStreamerInstance {
  int id;
  int port;
  std::string pipelineString;
  GstElement* pipeline;
  int width;
  int height;
  GLuint textureId;
  GstSample *sample;
  GMutex mutex;
};

std::vector<GStreamerInstance*> gstreamerInstances;

GStreamerInstance* GetGStreamerInstance(int instanceId) {
  auto it = std::find_if(gstreamerInstances.begin(), gstreamerInstances.end(), [instanceId](GStreamerInstance* instance) {
    return instance->id == instanceId;
  });

  if(it != gstreamerInstances.end()) {
    return *it;
  }
  else {
    DEBUG_LOG(unityLogPtr, "Unable to find GStreamer instance.");
    return nullptr;
  }
}

void gst_log_handler(GstDebugCategory *category, GstDebugLevel level,
                     const gchar *file, const gchar *function, gint line,
                     GObject *object, GstDebugMessage *message, gpointer user_data) {
    
    const gchar *debug_message = gst_debug_message_get(message);
    
    std::string formatted = std::string("[GStreamer] ") + debug_message;

    IUnityLog* unityLogPtr = static_cast<IUnityLog*>(user_data);
    
    if (!unityLogPtr) {
        fprintf(stdout, "%s\n", formatted.c_str());
        return;
    }

    DEBUG_LOG(unityLogPtr, formatted.c_str());
}

extern "C" UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GLuint InitGStreamerTexture(GStreamerInstance *instance) {
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, instance->width, instance->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

  return texture;
}

extern "C" UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API int CreateGStreamerInstance(int width, int height, int port, const char* pipeline) {
  DEBUG_LOG(unityLogPtr, "Strings");
  DEBUG_LOG(unityLogPtr, pipeline);
  DEBUG_LOG(unityLogPtr, std::string(pipeline));

  auto* instance = new GStreamerInstance();
  gstreamerInstances.push_back(instance);
  instance->id = id;
  instance->port = port;
  id += 1000;
  instance->width = width;
  instance->height = height;
  instance->pipelineString = std::string(pipeline);
  instance->sample = nullptr;
  g_mutex_init(&instance->mutex);
  return instance->id;
}

void InitGStreamerInstance(GStreamerInstance *instance) {
  instance->textureId = InitGStreamerTexture(instance);
}

extern "C" UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GLuint GetBackingTexture(int instanceId) {
  GStreamerInstance *instance = GetGStreamerInstance(instanceId);
  return instance->textureId;
}

extern "C"
{
  UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfacesPtr) {
    unityLogPtr = unityInterfacesPtr->Get<IUnityLog>();
    gst_init(nullptr, nullptr);
    gst_debug_set_default_threshold(GST_LEVEL_INFO);
    gst_debug_add_log_function(gst_log_handler, unityLogPtr, nullptr);

    s_UnityInterfaces = unityInterfacesPtr;
    s_Graphics = s_UnityInterfaces->Get<IUnityGraphics>();
    DEBUG_LOG(unityLogPtr, "Plugin loaded!");
  }
  
  UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API UnityPluginUnload() {
    DEBUG_LOG(unityLogPtr, "PLUGIN UNLOADING");
    for(auto* instance : gstreamerInstances) {
      if(instance->pipeline) {
        gst_element_set_state(instance->pipeline, GST_STATE_NULL);
        gst_object_unref(instance->pipeline);
      }
      
      g_mutex_lock(&instance->mutex);
      if(instance->sample) {
        gst_sample_unref(instance->sample);
      }
      g_mutex_unlock(&instance->mutex);
      
      if(instance->textureId) {
        glDeleteTextures(1, &instance->textureId);
      }
      
      g_mutex_clear(&instance->mutex);
      delete instance;
    }
    
    gstreamerInstances.clear();
    id = 1000;
    
    gst_debug_remove_log_function(gst_log_handler);
        
    unityLogPtr = nullptr;
    s_UnityInterfaces = nullptr;
    s_Graphics = nullptr;
  }
}

static GstFlowReturn on_new_sample(GstElement *sink, gpointer user_data) {
  GStreamerInstance *instance = (GStreamerInstance*)user_data;
  GstSample *sample;
  
  g_signal_emit_by_name(sink, "pull-sample", &sample);

  g_mutex_lock(&instance->mutex);
  if(instance->sample) {
    gst_sample_unref(instance->sample);
  }

  instance->sample = sample;
  g_mutex_unlock(&instance->mutex);
  
  return GST_FLOW_OK;
}

void UpdateTexture(GStreamerInstance *instance) {
  g_mutex_lock(&instance->mutex);
  if(instance->sample) {
    GstBuffer *buffer = gst_sample_get_buffer(instance->sample);
    GstMapInfo map;
    
    if (gst_buffer_map(buffer, &map, GST_MAP_READ)) {
      glBindTexture(GL_TEXTURE_2D, instance->textureId);
      glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 
                      instance->width, instance->height,
                      GL_RGBA, GL_UNSIGNED_BYTE, 
                      map.data);
      glFlush();
      
      gst_buffer_unmap(buffer, &map);
    }
    
    gst_sample_unref(instance->sample);
    instance->sample = nullptr;
  }
  g_mutex_unlock(&instance->mutex);
}

extern "C" UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API void StartGStreamer(GStreamerInstance* instance) {
  char pipelineStr[1024];
  snprintf(pipelineStr, sizeof(pipelineStr), instance->pipelineString.c_str(), instance->port);

  DEBUG_LOG(unityLogPtr, pipelineStr);

  DEBUG_LOG(unityLogPtr, "Pipeline ready");

  GError* error = nullptr;
  instance->pipeline = gst_parse_launch(pipelineStr, &error);
  if (!instance->pipeline) {
    DEBUG_LOG(unityLogPtr, "Failed to parse GStreamer pipeline!");
    if (error) {
        DEBUG_LOG(unityLogPtr, error->message);
        g_error_free(error);
    }
    return;
  }

  DEBUG_LOG(unityLogPtr, "After parse launch");

  GstElement *sink = gst_bin_get_by_name(GST_BIN(instance->pipeline), "unity");
  DEBUG_LOG(unityLogPtr, "Got appsink");

  // appsink frame callback
  g_signal_connect(sink, "new-sample", G_CALLBACK(on_new_sample), instance);

  DEBUG_LOG(unityLogPtr, "Signal connected");

  gst_element_set_state(instance->pipeline, GST_STATE_PLAYING);

  DEBUG_LOG(unityLogPtr, "Pipeline playing");

  g_object_unref(sink);
}

void UNITY_INTERFACE_API OnRenderEvent(int eventId) {  
  int instanceId = (eventId / 1000) * 1000;
  int action = eventId - instanceId;

  GStreamerInstance *instance;
  if(eventId >= 1000) {
    instance = GetGStreamerInstance(instanceId);
    if(instance == nullptr) {
      DEBUG_LOG(unityLogPtr, "Couldn't find GStreamer instance.");
      return;
    }
  }

  if(action != 3) {
    DEBUG_LOG_FORMAT(unityLogPtr, "Running action: %d", action);
  }

  switch(action) {
    case 1:
      InitGStreamerInstance(instance);
      break;
    case 2:
      StartGStreamer(instance);
      break;
    case 3:
      UpdateTexture(instance);
      break;
  }
}

extern "C" UnityRenderingEvent UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetRenderEventFunc() {
    return OnRenderEvent;
}
