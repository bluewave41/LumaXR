#include "IUnityInterface.h"
#include "IUnityGraphics.h"
#include "IUnityLog.h"

#include <gst/gst.h>
#include <gst/app/gstappsink.h>

#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include <cstdint>
#include <cstdarg>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <dlfcn.h>
#include <android/log.h>
#include <pthread.h>

static IUnityLog* unityLogPtr = nullptr;

static IUnityInterfaces *s_UnityInterfaces = nullptr;
static IUnityGraphics *s_Graphics = nullptr;
JavaVM *g_java_vm = nullptr;

/*static int id = 1000;

struct GStreamerInstance {
  int id;
  int port;
  std::string pipelineString;
  GstElement* pipeline;
  int width;
  int height;
  GLuint textureId;
  GstSample *sample;
  pthread_mutex_t mutex;
  ANativeWindow* nativeWindow;
};

std::vector<GStreamerInstance*> gstreamerInstances;*/

/*GStreamerInstance* GetGStreamerInstance(int instanceId) {
  auto it = std::find_if(gstreamerInstances.begin(), gstreamerInstances.end(), [instanceId](GStreamerInstance* instance) {
    return instance->id == instanceId;
  });

  if(it != gstreamerInstances.end()) {
    return *it;
  }
  else {
    __android_log_print(ANDROID_LOG_INFO, "LumaXR", "Unable to find GStreamer instance.");
    return nullptr;
  }
}*/

/*extern "C" JNIEXPORT void JNICALL Java_com_bluewave41_lumaxr_UnitySurface_nativeInit(JNIEnv *env, jclass clazz, jint instanceId) {
  if(!g_java_vm) {
    env->GetJavaVM(&g_java_vm);
  }

  GStreamerInstance *instance = GetGStreamerInstance(instanceId);
  if(!instance) {
    __android_log_print(ANDROID_LOG_INFO, "LumaXR", "Native init %d", instanceId);
    return;
  }

  __android_log_print(ANDROID_LOG_INFO, "LumaXR", "Stored plugin instance");
}*/

/*ANativeWindow* CreateSurfaceFromUnityTexture(int textureId) {
  ANativeWindow* nativeWindow = nullptr;
  
  JNIEnv *env = nullptr;

  g_java_vm->AttachCurrentThread(&env, nullptr);

  jclass cls = env->FindClass("com/bluewave41/LumaXR/GStreamer");
  if(!cls) {
    __android_log_print(ANDROID_LOG_INFO, "LumaXR", "Failed to find Java class");
    return nullptr;
  }

  jmethodID ctor = env->GetMethodID(cls, "<init>", "(I)V");
  if(!ctor) {
    __android_log_print(ANDROID_LOG_INFO, "LumaXR", "Failed to find Java constructor");
    return nullptr;
  }

  jobject obj = env->NewObject(cls, ctor, textureId);
  if(!obj) {
    __android_log_print(ANDROID_LOG_INFO, "LumaXR", "Failed to find Java object");
    return nullptr;
  }

  jmethodID getSurface = env->GetMethodID(cls, "getSurface", "()Landroid/View/Surface;");
  if(!getSurface) {
    __android_log_print(ANDROID_LOG_INFO, "LumaXR", "Failed to find Java getSurface()");
    return nullptr;
  }

  jobject surface = env->CallObjectMethod(obj, getSurface);
  if(!surface) {
    __android_log_print(ANDROID_LOG_INFO, "LumaXR", "Failed to call Java getSurface()");
    return nullptr;
  }

  nativeWindow = ANativeWindow_fromSurface(env, surface);

  jobject globalObj = env->NewGlobalRef(obj);

  return nativeWindow;
}*/

/*void gst_log_handler(GstDebugCategory *category, GstDebugLevel level,
                     const gchar *file, const gchar *function, gint line,
                     GObject *object, GstDebugMessage *message, gpointer user_data) {
    
    const gchar *debug_message = gst_debug_message_get(message);
    
    std::string formatted = std::string("[GStreamer] ") + debug_message;

    IUnityLog* unityLogPtr = static_cast<IUnityLog*>(user_data);
    
    if (!unityLogPtr) {
        fprintf(stdout, "%s\n", formatted.c_str());
        return;
    }
    
    __android_log_print(ANDROID_LOG_INFO, "LumaXR", "%s", formatted.c_str());
}*/

/*extern "C" UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GLuint InitGStreamerTexture(GStreamerInstance *instance) {
  __android_log_print(ANDROID_LOG_INFO, "LumaXR", "%d %d", instance->width, instance->height);
  
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, instance->width, instance->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

  return texture;
}

extern "C" UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API int CreateGStreamerInstance(int width, int height, int port, const char* pipeline) {
  __android_log_print(ANDROID_LOG_INFO, "LumaXR", "%s", pipeline);
  auto* instance = new GStreamerInstance();
  gstreamerInstances.push_back(instance);
  instance->id = id;
  instance->port = port;
  id += 1000;
  instance->width = width;
  instance->height = height;
  instance->pipelineString = std::string(pipeline);
  instance->sample = nullptr;
  pthread_mutex_init(&instance->mutex, NULL);
  return instance->id;
}

void InitGStreamerInstance(GStreamerInstance *instance) {
  instance->textureId = InitGStreamerTexture(instance);
  //instance->nativeWindow = CreateSurfaceFromUnityTexture(instance->textureId);
}*/

/*extern "C" UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GLuint GetTextureId(int instanceId) {
  GStreamerInstance *instance = GetGStreamerInstance(instanceId);
  return instance->textureId;
}*/

/*extern "C"
{
  UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfacesPtr) {
    //unityLogPtr = unityInterfacesPtr->Get<IUnityLog>();
    //gst_init(nullptr, nullptr);
    //gst_debug_set_default_threshold(GST_LEVEL_INFO);
    //gst_debug_add_log_function(gst_log_handler, unityLogPtr, nullptr);

    //s_UnityInterfaces = unityInterfacesPtr;
    //s_Graphics = s_UnityInterfaces->Get<IUnityGraphics>();
    __android_log_print(ANDROID_LOG_INFO, "LumaXR", "Plugin loaded!");

  }
  
  UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API UnityPluginUnload() {
    __android_log_print(ANDROID_LOG_INFO, "LumaXR", "Plugin unloaded!");
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
}*/

/*static GstFlowReturn on_new_sample(GstElement *sink, gpointer user_data) {
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
}*/

/*void UpdateTexture(GStreamerInstance *instance) {
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

      GLenum err = glGetError();
      if (err != GL_NO_ERROR) {
        DEBUG_LOG_FORMAT(unityLogPtr, "GL Error: 0x%x", err);
      }
      
      gst_buffer_unmap(buffer, &map);
    }
    
    gst_sample_unref(instance->sample);
    instance->sample = nullptr;
  }
  g_mutex_unlock(&instance->mutex);
}*/

/*extern "C" UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API void StartGStreamer(GStreamerInstance* instance) {
  char pipelineStr[1024];
  snprintf(pipelineStr, sizeof(pipelineStr), instance->pipelineString.c_str(), instance->port);

  __android_log_print(ANDROID_LOG_INFO, "LumaXR", "%s", pipelineStr);
  __android_log_print(ANDROID_LOG_INFO, "LumaXR", "Pipeline ready");

  GError* error = nullptr;
  instance->pipeline = gst_parse_launch(pipelineStr, &error);
  if (!instance->pipeline) {
    __android_log_print(ANDROID_LOG_INFO, "LumaXR", "Failed to parse GStreamer pipeline");
    if (error) {
        __android_log_print(ANDROID_LOG_INFO, "LumaXR", "%s", error->message);
        g_error_free(error);
    }
    return;
  }

  __android_log_print(ANDROID_LOG_INFO, "LumaXR", "After parse launch");

  //GstElement *sink = gst_bin_get_by_name(GST_BIN(instance->pipeline), "unity");
  //DEBUG_LOG(unityLogPtr, "Got appsink");

  // appsink frame callback
  //g_signal_connect(sink, "new-sample", G_CALLBACK(on_new_sample), instance);

  //DEBUG_LOG(unityLogPtr, "Signal connected");

  gst_element_set_state(instance->pipeline, GST_STATE_PLAYING);

  //DEBUG_LOG(unityLogPtr, "Pipeline playing");

  //g_object_unref(sink);
}*/

/*void UNITY_INTERFACE_API OnRenderEvent(int eventId) {  
  int instanceId = (eventId / 1000) * 1000;
  int action = eventId - instanceId;

  GStreamerInstance *instance;
  if(eventId >= 1000) {
    instance = GetGStreamerInstance(instanceId);
    if(instance == nullptr) {
      __android_log_print(ANDROID_LOG_INFO, "LumaXR", "Couldn't find GStreamer instance.");
      return;
    }
  }

  if(action != 3) {
    __android_log_print(ANDROID_LOG_INFO, "LumaXR", "Running action: %d", action);
  }

  switch(action) {
    case 1:
      //InitGStreamerInstance(instance);
      break;
    case 2:
      //StartGStreamer(instance);
      break;
    case 3:
      //UpdateTexture(instance);
      break;
  }
}

extern "C" UnityRenderingEvent UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetRenderEventFunc() {
    return OnRenderEvent;
}*/
