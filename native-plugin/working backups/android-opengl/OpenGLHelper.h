#pragma once
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include "GStreamerInstance.h"

namespace OpenGLHelper {
    extern GLuint g_oesShader;
    extern GLint g_textureLoc;
    extern GLint g_matrixLoc;

    void init();
    void glInit();
    GLuint CreateOESShaderProgram();
}