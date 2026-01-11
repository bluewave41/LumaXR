#!/bin/bash

NDK=/home/matthew/Unity/Hub/Editor/6000.3.2f1/Editor/Data/PlaybackEngines/AndroidPlayer/NDK
TARGET_API=32
GSTREAMER=./gstreamer/arm64
OUTPUT=libgstplugin.so

CLANG=$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/clang++

GST_STATIC_LIBS="
$GSTREAMER/lib/libgstreamer-1.0.a
$GSTREAMER/lib/libgstapp-1.0.a
$GSTREAMER/lib/libgstgl-1.0.a
$GSTREAMER/lib/libgstbase-1.0.a
$GSTREAMER/lib/libgmodule-2.0.a
$GSTREAMER/lib/libgobject-2.0.a
$GSTREAMER/lib/libglib-2.0.a
$GSTREAMER/lib/libffi.a
$GSTREAMER/lib/libiconv.a
$GSTREAMER/lib/libpcre2-8.a
$GSTREAMER/lib/libintl.a
$GSTREAMER/lib/libgstvideo-1.0.a
$GSTREAMER/lib/liborc-0.4.a
$GSTREAMER/lib/libgstrtp-1.0.a
$GSTREAMER/lib/libgstaudio-1.0.a
$GSTREAMER/lib/libgio-2.0.a
$GSTREAMER/lib/libgsttag-1.0.a
$GSTREAMER/lib/libgstnet-1.0.a
$GSTREAMER/lib/libgstpbutils-1.0.a
$GSTREAMER/lib/libgstphotography-1.0.a
$GSTREAMER/lib/libgstcodecparsers-1.0.a
$GSTREAMER/lib/libpng16.a
$GSTREAMER/lib/libgraphene-1.0.a
$GSTREAMER/lib/libgstcontroller-1.0.a
$GSTREAMER/lib/libjpeg.a
$GSTREAMER/lib/gstreamer-1.0/libgstudp.a
$GSTREAMER/lib/gstreamer-1.0/libgstrtp.a
$GSTREAMER/lib/gstreamer-1.0/libgstrtpmanager.a
$GSTREAMER/lib/gstreamer-1.0/libgstandroidmedia.a
$GSTREAMER/lib/gstreamer-1.0/libgstapp.a
$GSTREAMER/lib/gstreamer-1.0/libgstvideoparsersbad.a
$GSTREAMER/lib/gstreamer-1.0/libgstopengl.a
$GSTREAMER/lib/gstreamer-1.0/libgstcoreelements.a
$GSTREAMER/lib/gstreamer-1.0/libgstvideotestsrc.a
$GSTREAMER/lib/gstreamer-1.0/libgstdebugutilsbad.a
"

# Add -Wl,--no-undefined to catch missing symbols
$CLANG \
  --target=aarch64-linux-android$TARGET_API \
  -fPIC \
  -shared \
  -static-libstdc++ \
  gst_plugin.cpp OpenGLHelper.cpp Logger.cpp GStreamerInstance.cpp GstUtils.cpp XRHelper.cpp \
  -o $OUTPUT \
  -I$NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include \
  -I$GSTREAMER/include/gstreamer-1.0 \
  -I$GSTREAMER/lib/gstreamer-1.0/include \
  -I$GSTREAMER/include/glib-2.0 \
  -I$GSTREAMER/lib/glib-2.0/include \
  -L. -lopenxr_loader \
  -Wl,--no-undefined \
  -Wl,--whole-archive \
      $GST_STATIC_LIBS \
  -Wl,--no-whole-archive \
  -lEGL -lGLESv3 -lz -lm -latomic -ldl -llog -landroid -lc++
