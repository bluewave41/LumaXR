#include "GStreamerInstance.h"
#include "Logger.h"
#include "JNIUtils.h"

static int g_id = 1000;
std::vector<GStreamerInstance*> gstreamerInstances;

GStreamerInstance::GStreamerInstance(int _width, int _height, int _port, std::string _pipelineString)
    : id(g_id),
    port(_port),
    pipelineString(_pipelineString),
    pipeline(nullptr),
    width(_width),
    height(_height)
{
  g_id += 1000;
}

GStreamerInstance::~GStreamerInstance() {
  if(pipeline) {
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    pipeline = nullptr;
  }
  if(nativeWindow) {
    ANativeWindow_release(nativeWindow);
    nativeWindow = nullptr;
  }
  if(surface) {
    JNIEnv *env = JNIUtils::Attach();
    env->DeleteGlobalRef(surface);
    surface = nullptr;
    JNIUtils::Detatch();
  }
}

GStreamerInstance* GetGStreamerInstance(int instanceId) {
  auto it = std::find_if(gstreamerInstances.begin(), gstreamerInstances.end(), [instanceId](GStreamerInstance* instance) {
    return instance->id == instanceId;
  });

  if(it != gstreamerInstances.end()) {
    return *it;
  }
  else {
    Logger::log("Unable to find GStreamer instance.");
    Logger::logFormat("%d", instanceId);
    return nullptr;
  }
}