#include "GStreamerInstance.h"
#include "Logger.h"
#include "OpenGLHelper.h"

static int g_id = 1000;
std::vector<GStreamerInstance*> gstreamerInstances;

GStreamerInstance::GStreamerInstance(int _width, int _height, int _port, std::string _pipelineString)
    : id(g_id),
    port(_port),
    pipelineString(_pipelineString),
    pipeline(nullptr),
    width(_width),
    height(_height),
    textureId(0),
    oesTextureId(0),
    sample(nullptr),
    nativeWindow(nullptr),
    surfaceTexture(nullptr),
    fbo(0),
    samples(0),
    lastTime(0),
    swapchain(XRHelper::CreateSwapchain(width, height))
{
  swapchainImages = XRHelper::QueryImages(swapchain);
  pthread_mutex_init(&mutex, nullptr);
  g_id += 1000;
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
    return nullptr;
  }
}