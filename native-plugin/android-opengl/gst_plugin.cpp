//https://github.com/sealfoss/GstUnityPlugin/blob/main/source/GstStreamController.cpp

/*GList *decoders = gst_element_factory_list_get_elements(GST_ELEMENT_FACTORY_TYPE_DECODABLE,
                                                        GST_RANK_MARGINAL);

// Iterate through the list
for (GList *iter = decoders; iter != NULL; iter = iter->next) {
    GstElementFactory *factory = (GstElementFactory *) iter->data;

    // Get the factory name suitable for use in a string pipeline
    const gchar *name = gst_element_get_name(factory);

    // Print the factory name
    g_print("Decoder: %s\n", nam);
}*/

//amcviddec-c2qtiavcdecoderlowlatency

#include "IUnityInterface.h"
#include "IUnityGraphics.h"
#include "IUnityLog.h"
#include "Logger.h"
#include "GStreamerInstance.h"
#include "GstUtils.h"
#include "JNIUtils.h"

#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <gst/video/videooverlay.h>
#include <gst/gl/egl/gstgldisplay_egl.h>

#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/surface_texture.h>
#include <android/surface_texture_jni.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <cstdint>
#include <cstdarg>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <dlfcn.h>
#include <pthread.h>
#include <GLES3/gl32.h>
#include <GLES3/gl3.h>

extern "C" {
  GST_PLUGIN_STATIC_DECLARE(app);
  GST_PLUGIN_STATIC_DECLARE(udp);
  GST_PLUGIN_STATIC_DECLARE(rtp);
  GST_PLUGIN_STATIC_DECLARE(rtpmanager);
  GST_PLUGIN_STATIC_DECLARE(androidmedia);
  GST_PLUGIN_STATIC_DECLARE(videoparsersbad);
  GST_PLUGIN_STATIC_DECLARE(coreelements);
  GST_PLUGIN_STATIC_DECLARE(opengl);
  GST_PLUGIN_STATIC_DECLARE(videotestsrc);
  GST_PLUGIN_STATIC_DECLARE(debugutilsbad);
  GST_PLUGIN_STATIC_DECLARE(videofilter);
}

void register_static_plugins() {
  GST_PLUGIN_STATIC_REGISTER(app);
  GST_PLUGIN_STATIC_REGISTER(udp);
  GST_PLUGIN_STATIC_REGISTER(rtp);
  GST_PLUGIN_STATIC_REGISTER(rtpmanager);
  GST_PLUGIN_STATIC_REGISTER(androidmedia);
  GST_PLUGIN_STATIC_REGISTER(videoparsersbad);
  GST_PLUGIN_STATIC_REGISTER(coreelements);
  GST_PLUGIN_STATIC_REGISTER(opengl);
  GST_PLUGIN_STATIC_REGISTER(videotestsrc);
  GST_PLUGIN_STATIC_REGISTER(debugutilsbad);
  GST_PLUGIN_STATIC_REGISTER(videofilter);
}

static IUnityInterfaces *s_UnityInterfaces = nullptr;
static IUnityGraphics *s_Graphics = nullptr;

/**
 * Sets the Java VM in our native plugin. OnLoad is already called by GStreamer and we can't have multiple
 * OnLoad functions thus why this exists.
 */
extern "C" JNIEXPORT void JNICALL
Java_com_bluewave41_lumaxr_NativeBridge_setJavaVM(JNIEnv* env, jclass clazz) {
  Logger::log("Setting native VM");
  env->GetJavaVM(&JNIUtils::g_vm);
}

/**
 * Receives the Surface from the CompositorLayer and stores it on the instance. The NativeWindow needs to be
 * created on the render thread and we can't pass the surface along with the instanceId.
 */
extern "C" UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API void SetNativeSurface(int instanceId, void* surface) {
  Logger::log("Setting native surface");

  if (!surface) {
    Logger::log("ERROR: Surface is null");
    return;
  }

  GStreamerInstance* instance = GetGStreamerInstance(instanceId);
  jobject javaSurface = reinterpret_cast<jobject>(surface);
  if (!javaSurface) {
    Logger::log("ERROR: Invalid Java surface");
    return;
  }

  JNIEnv *env = JNIUtils::Attach();
  instance->surface = env->NewGlobalRef(javaSurface);
  Logger::log("Native surface set successfully");
}

/**
 * Prints GStreamer log messages the console if attached to the bus
 */
void gst_log_handler(GstDebugCategory *category, GstDebugLevel level,
                     const gchar *file, const gchar *function, gint line,
                     GObject *object, GstDebugMessage *message, gpointer user_data) {
  const gchar *debug_message = gst_debug_message_get(message);
  std::string formatted = std::string("[GStreamer] ") + debug_message;    
  Logger::logFormat("%s", formatted.c_str());
}

/**
 * Creates a GStreamer instance class
 */
extern "C" UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API int CreateGStreamerInstance(int width, int height, int port, const char* pipeline) {
  Logger::logFormat("%s", pipeline);
  auto* instance = new GStreamerInstance(width, height, port, std::string(pipeline));
  gstreamerInstances.push_back(instance);
  return instance->id;
}

/**
 * Initializes a GStreamer instance. Must be run on the render thread
 */
void InitGStreamerInstance(GStreamerInstance *instance) {
  JNIEnv* env = JNIUtils::Attach();
  instance->nativeWindow = ANativeWindow_fromSurface(env, instance->surface);
  Logger::log("Native window set successfully");
}

extern "C"
{
  UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfacesPtr) {
    IUnityLog * unityLogPtr = unityInterfacesPtr->Get<IUnityLog>();

    gst_init(nullptr, nullptr);
    register_static_plugins();
    gst_debug_set_default_threshold(GST_LEVEL_INFO);
    gst_debug_add_log_function(gst_log_handler, unityLogPtr, nullptr);

    Logger::init(unityLogPtr);
    s_UnityInterfaces = unityInterfacesPtr;
    s_Graphics = s_UnityInterfaces->Get<IUnityGraphics>();
    Logger::log("Plugin loaded");
  }
  
  UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API UnityPluginUnload() {
    Logger::log("Plugin unloaded");
    /*for(auto* instance : gstreamerInstances) {
      if(instance->pipeline) {
        gst_element_set_state(instance->pipeline, GST_STATE_NULL);
        gst_object_unref(instance->pipeline);
      }
      
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
    s_Graphics = nullptr;*/
  }
}

/**
 * Feeds our ANativeWindow to the glimagesink so it can render directly to it and handle any color
 * conversions/transformations automatically.
 */
static GstBusSyncReply create_window (GstBus * bus, GstMessage * message, GStreamerInstance *instance) {
  if (!gst_is_video_overlay_prepare_window_handle_message (message)) {
    return GST_BUS_PASS;
  }
  Logger::log("Feeding ANativeWindow to GStreamer");
  Logger::logFormat("Native window: %p", instance->nativeWindow);

  GstElement* glimagesink = gst_bin_get_by_name(GST_BIN(instance->pipeline), "unity");
  gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(glimagesink), (guintptr)instance->nativeWindow);
  gst_object_unref(glimagesink);

  return GST_BUS_DROP;
}

/**
 * Starts the pipeline to receive frames over UDP
 */
extern "C" UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API void StartGStreamer(GStreamerInstance* instance) {
  char pipelineStr[1024];
  snprintf(pipelineStr, sizeof(pipelineStr), instance->pipelineString.c_str(), instance->port);

  Logger::logFormat("Pipeline string: %s", pipelineStr);
  
  GError* error = nullptr;
  instance->pipeline = gst_parse_launch(pipelineStr, &error);
  if (!instance->pipeline) {
    Logger::log("Failed to parse GStreamer pipeline");
    if (error) {
      Logger::logFormat("Error: %s", error->message);
      g_error_free(error);
    }
    return;
  }

  GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(instance->pipeline));
  gst_bus_set_sync_handler(bus, (GstBusSyncHandler) create_window, instance, NULL);
  gst_object_unref(bus);

  gst_element_set_state(instance->pipeline, GST_STATE_PLAYING);
  Logger::log("After started");
}

extern "C" UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API void StopPipeline(int instanceId) {
  GStreamerInstance *instance = GetGStreamerInstance(instanceId);
  if(!instance) {
    return;
  }

  gst_element_set_state(instance->pipeline, GST_STATE_NULL);
  gst_object_unref(instance->pipeline);
}

extern "C" UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API void RestartStream(GStreamerInstance *instance) {
  InitGStreamerInstance(instance);
  StartGStreamer(instance);
}

/**
 * Defines the RenderEvent function which is called by Unity. This runs on the
 * render thread. Instance ids are passed in incremented by the event id to
 * differentiate events.
 */
void UNITY_INTERFACE_API OnRenderEvent(int eventId) {  
  int instanceId = (eventId / 1000) * 1000;
  int action = eventId - instanceId;

  GStreamerInstance *instance;
  if(eventId >= 1000) {
    instance = GetGStreamerInstance(instanceId);
    if(instance == nullptr) {
      return;
    }
  }

  if(action != 3) {
    Logger::logFormat("Running action: %d", action);
  }

  switch(action) {
    case 1:
      InitGStreamerInstance(instance);
      break;
    case 2:
      StartGStreamer(instance);
      break;
    case 3:
      RestartStream(instance);
      break;
  }
}

extern "C" UnityRenderingEvent UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetRenderEventFunc() {
    return OnRenderEvent;
}