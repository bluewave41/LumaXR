#include "OpenGLHelper.h"
#include "Logger.h"

const char* vertexShaderSource =
"#version 300 es\n"
"#extension GL_OES_EGL_image_external_essl3 : require\n"
"precision mediump float;\n"
"out vec2 vTexCoord;\n"
"uniform mat4 uTexMatrix;\n"
"void main() {\n"
"    vec2 pos[4] = vec2[](\n"
"        vec2(-1.0, -1.0),\n"
"        vec2( 1.0, -1.0),\n"
"        vec2(-1.0,  1.0),\n"
"        vec2( 1.0,  1.0)\n"
"    );\n"
"    vec2 uv[4] = vec2[](\n"
"        vec2(0.0, 1.0),\n"
"        vec2(1.0, 1.0),\n"
"        vec2(0.0, 0.0),\n"
"        vec2(1.0, 0.0)\n"
"    );\n"
"    gl_Position = vec4(pos[gl_VertexID], 0.0, 1.0);\n"
"    vec4 transformed = uTexMatrix * vec4(uv[gl_VertexID], 0.0, 1.0);\n"
"    vTexCoord = transformed.xy;\n"
"}\n";

const char* fragmentShaderSource =
"#version 300 es\n"
"#extension GL_OES_EGL_image_external_essl3 : require\n"
"precision mediump float;\n"
"in vec2 vTexCoord;\n"
"out vec4 fragColor;\n"
"uniform samplerExternalOES uTexture;\n"
"void main() {\n"
"    fragColor = texture(uTexture, vTexCoord);\n"
"}\n";

namespace OpenGLHelper {
    GLuint g_oesShader;
    GLint g_textureLoc;
    GLint g_matrixLoc;

    void init() {
        g_oesShader = CreateOESShaderProgram();
    }

    void glInit() {
        g_textureLoc = glGetUniformLocation(OpenGLHelper::g_oesShader, "uTexture");
        g_matrixLoc = glGetUniformLocation(OpenGLHelper::g_oesShader, "uTexMatrix");
    }

    GLuint CreateOESShaderProgram() {
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
        glCompileShader(vertexShader);
        
        // Check vertex shader compilation
        GLint success;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            Logger::log("Failed to create shader vertex");
            char infoLog[512];
            glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        }
        
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
        glCompileShader(fragmentShader);
        
        // Check fragment shader compilation
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            Logger::log("Failed to create shader fragment");
            char infoLog[512];
            glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        }
        
        GLuint shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        
        // Check linking
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            Logger::log("Failed to link shader");
            char infoLog[512];
            glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        }
        
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        
        return shaderProgram;
    }
}