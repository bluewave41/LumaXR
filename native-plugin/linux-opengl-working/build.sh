g++ gst_plugin.cpp -o __gstplugin.so -shared -fPIC \
  $(pkg-config --cflags gstreamer-1.0 gstreamer-app-1.0 gstreamer-gl-1.0) \
  -I/usr/lib/x86_64-linux-gnu/gstreamer-1.0/include \
  $(pkg-config --libs gstreamer-1.0 gstreamer-app-1.0 gstreamer-gl-1.0) \
  -lgstgl-1.0 \
  -lEGL \
  -lGL
