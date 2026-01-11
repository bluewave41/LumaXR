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
#include "OpenGLHelper.h"
#include "Logger.h"
#include "GStreamerInstance.h"
#include "GstUtils.h"
#include "XRHelper.h"

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
}

static EGLContext g_unityContext = EGL_NO_DISPLAY;
static EGLDisplay g_unityDisplay = EGL_NO_DISPLAY;
static GstGLContext *sharedContext;

static IUnityInterfaces *s_UnityInterfaces = nullptr;
static IUnityGraphics *s_Graphics = nullptr;

extern "C" UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API void SetXRInstance(uint64_t instance) {
  XRHelper::SetInstance(instance);
}

extern "C" UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API void SetXRSession(uint64_t session) {
  XRHelper::SetSession(session);
}

void GLInit() {
  g_unityDisplay = eglGetCurrentDisplay();
  g_unityContext = eglGetCurrentContext();

  Logger::logFormat("Unity context %p", g_unityContext);
  Logger::logFormat("Unity display %p", g_unityDisplay);

  Logger::log("Before display");
  GstGLDisplayEGL *display = gst_gl_display_egl_new_with_egl_display(g_unityDisplay);

  Logger::log("Before shared context");
  sharedContext = gst_gl_context_new_wrapped(
    GST_GL_DISPLAY_CAST(display),
    (guintptr)g_unityContext,
    GST_GL_PLATFORM_EGL,
    GST_GL_API_GLES2
  );

  Logger::log("Before activate");
  gst_gl_context_activate(sharedContext, TRUE);
  GError *error;
  Logger::log("After activate");
  if (!gst_gl_context_fill_info (sharedContext, &error)) {
    Logger::log("Failed to retrieve context info");
    gst_gl_context_activate (sharedContext, FALSE);
    return;
  }

  Logger::log("Before false activate");
  gst_gl_context_activate (sharedContext, FALSE);

  Logger::log("GL has been init successfully.");
}

static gboolean
sync_bus_call (GstBus *bus, GstMessage *msg, gpointer    data)
{
  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_NEED_CONTEXT:
    {
      const gchar *context_type;
      GstContext *context = NULL;
     
      gst_message_parse_context_type (msg, &context_type);
      Logger::logFormat("Got need context %s", context_type);

      if (g_strcmp0 (context_type, "gst.gl.app_context") == 0) {
        GstGLContext *gl_context = sharedContext;
        GstStructure *s;

        Logger::log("Setting context...");
        context = gst_context_new ("gst.gl.app_context", TRUE);
        Logger::log("Made new context");
        s = gst_context_writable_structure (context);
        Logger::log("Made writable context");
        gst_structure_set (s, "context", GST_TYPE_GL_CONTEXT, gl_context, NULL);
        Logger::log("Set GL context");
        gst_element_set_context (GST_ELEMENT (msg->src), context);
        Logger::log("Set context");
      }
      if (context) {
        gst_context_unref (context);
      }
      break;
    }
    default:
      break;
  }

  return FALSE;
}

void CopyOESToUnityTexture(GStreamerInstance* instance) {
    if (!instance->sample) {
        return;
    }

    // Get the source OpenGL texture from the GStreamer sample
    GstBuffer* buffer = gst_sample_get_buffer(instance->sample);
    GstMemory* mem = gst_buffer_peek_memory(buffer, 0);
    if (!gst_is_gl_memory(mem))
        return;

    GstGLMemory* glMem = GST_GL_MEMORY_CAST(mem);
    GLuint srcTex = gst_gl_memory_get_texture_id(glMem);
    
    GstGLTextureTarget fmt = gst_gl_memory_get_texture_target(glMem);
    Logger::logFormat("Texture target: %d", fmt);
    Logger::logFormat("Src tex: %d", srcTex);
    Logger::logFormat("Instance texture: %d", instance->textureId);

    glCopyImageSubData(
      srcTex, GL_TEXTURE_2D, 0, 0, 0, 0,
      instance->textureId, GL_TEXTURE_2D, 0, 0, 0, 0,
      instance->width, instance->height, 1
    );
    
    // Cleanup
    gst_sample_unref(instance->sample);
    instance->sample = nullptr;

    Logger::log("Copied GLMemory texture via GPU");


  /*int r = ASurfaceTexture_updateTexImage(instance->surfaceTexture);
    
  float transformMatrix[16];
  ASurfaceTexture_getTransformMatrix(instance->surfaceTexture, transformMatrix);

  glBindFramebuffer(GL_FRAMEBUFFER, instance->fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, instance->textureId, 0);
  glViewport(0, 0, instance->width, instance->height);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(OpenGLHelper::g_oesShader);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_EXTERNAL_OES, instance->oesTextureId);
  glUniform1i(OpenGLHelper::g_textureLoc, 0);
  glUniformMatrix4fv(OpenGLHelper::g_matrixLoc, 1, GL_FALSE, transformMatrix);

  glDisable(GL_CULL_FACE);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glBindTexture(GL_TEXTURE_EXTERNAL_OES, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);*/
}

extern "C" JNIEXPORT void JNICALL Java_com_bluewave41_lumaxr_UnitySurface_nativeInit(JNIEnv *env, jobject thiz, jobject surfaceTexture, jobject surface, jint instanceId) {
  Logger::log("Native called");
  GStreamerInstance *instance = GetGStreamerInstance(instanceId);
  instance->nativeWindow = ANativeWindow_fromSurface(env, surface);
  instance->surfaceTexture = ASurfaceTexture_fromSurfaceTexture(env, surfaceTexture);
}

void gst_log_handler(GstDebugCategory *category, GstDebugLevel level,
                     const gchar *file, const gchar *function, gint line,
                     GObject *object, GstDebugMessage *message, gpointer user_data) {
    
    const gchar *debug_message = gst_debug_message_get(message);
    
    std::string formatted = std::string("[GStreamer] ") + debug_message;
    
    Logger::logFormat("%s", formatted.c_str());
}

extern "C" void InitGStreamerTexture(GStreamerInstance *instance) {
  glGenTextures(1, &instance->textureId);
  glBindTexture(GL_TEXTURE_2D, instance->textureId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, instance->width, instance->height, 
               0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  Logger::logFormat("Texture created %d", instance->textureId);
}

extern "C" UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API int CreateGStreamerInstance(int width, int height, int port, const char* pipeline) {
  Logger::logFormat("%s", pipeline);
  auto* instance = new GStreamerInstance(width, height, port, std::string(pipeline));
  gstreamerInstances.push_back(instance);
  return instance->id;
}

void InitGStreamerInstance(GStreamerInstance *instance) {
  OpenGLHelper::glInit();
  InitGStreamerTexture(instance);
}

extern "C" UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GLuint GetBackingTexture(int instanceId) {
  GStreamerInstance *instance = GetGStreamerInstance(instanceId);
  return instance->textureId;
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
    OpenGLHelper::init();
    s_UnityInterfaces = unityInterfacesPtr;
    s_Graphics = s_UnityInterfaces->Get<IUnityGraphics>();
    GLInit();
    Logger::log("Plugin loaded");
  }
  
  UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API UnityPluginUnload() {
    Logger::log("Plugin unloaded");
    /*for(auto* instance : gstreamerInstances) {
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
    s_Graphics = nullptr;*/
  }
}

 static GstBusSyncReply
 create_window (GstBus * bus, GstMessage * message, GStreamerInstance *instance)
 {
  if (!gst_is_video_overlay_prepare_window_handle_message (message)) {
    return GST_BUS_PASS;
  }
  Logger::log("Feeding ANativeWindow to GStreamer");
  Logger::logFormat("Surface texture: %p", instance->surfaceTexture);
  Logger::logFormat("Native window: %p", instance->nativeWindow);

  GstElement* glimagesink = gst_bin_get_by_name(GST_BIN(instance->pipeline), "unity");
  gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(glimagesink), (guintptr)instance->nativeWindow);
  gst_object_unref(glimagesink);

  return GST_BUS_DROP;
}

static GstFlowReturn on_new_sample(GstAppSink* sink, gpointer user_data)
{
  GStreamerInstance *instance = static_cast<GStreamerInstance*>(user_data);

  GstSample* sample = gst_app_sink_pull_sample(sink);
  if (!sample) {
    return GST_FLOW_OK;
  }

  if (instance->sample) {
    gst_sample_unref(instance->sample);
  }

  instance->sample = sample;

  Logger::log("Got a sample!");

  return GST_FLOW_OK;
}


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
  gst_bus_set_sync_handler(bus, (GstBusSyncHandler)sync_bus_call, NULL, NULL);
  //gst_bus_set_sync_handler(bus, (GstBusSyncHandler) create_window, instance, NULL);

  GstElement *appsink = gst_bin_get_by_name(GST_BIN(instance->pipeline), "unity");
  g_signal_connect(
    appsink,
    "new-sample",
    G_CALLBACK(on_new_sample),
    instance
  );

  glGenFramebuffers(1, &instance->fbo);
  gst_element_set_state(instance->pipeline, GST_STATE_PLAYING);
}

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
      CopyOESToUnityTexture(instance);
      break;
  }
}

extern "C" UnityRenderingEvent UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetRenderEventFunc() {
    return OnRenderEvent;
}
