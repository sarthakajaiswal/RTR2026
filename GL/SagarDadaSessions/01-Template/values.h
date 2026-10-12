#pragma once 

extern const char* vertexShaderSourceCode; 
extern const char* fragmentShaderSourceCode; 

extern const float pyramid_position[36]; 
extern const float pyramid_texCoords[24]; 

#ifdef _VALUES_C 

// Win32 headers
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <gl/glew.h> 	// this header file must be included before gl/GL.h 
#include <gl/GL.h> 

const char* vertexShaderSourceCode = 
    "#version 460 core\n" \
    "in vec4 aPosition;\n" \
    "in vec2 aTexCoord;\n" \
    "out vec2 out_TexCoord;\n" \
    "uniform mat4 uMVPMatrix;\n" \
    "void main(void) \n" \
    "{\n" \
    "gl_Position = uMVPMatrix * aPosition;\n" \
    "out_TexCoord = aTexCoord;\n" \
    "}\n"; 

const char* fragmentShaderSourceCode = 
    "#version 460 core\n" \
    "in vec2 out_TexCoord;\n" \
    "uniform sampler2D uTextureSampler;"
    "out vec4 FragColor;"
    "void main(void)\n" \
    "{\n" \
    "FragColor = texture(uTextureSampler, out_TexCoord);\n" \
    "}\n"; 

const float pyramid_position[] = 
{
    // front
    0.0f,  1.0f,  0.0f, // front-top
    -1.0f, -1.0f,  1.0f, // front-left
    1.0f, -1.0f,  1.0f, // front-right
    
    // right
    0.0f,  1.0f,  0.0f, // right-top
    1.0f, -1.0f,  1.0f, // right-left
    1.0f, -1.0f, -1.0f, // right-right

    // back
    0.0f,  1.0f,  0.0f, // back-top
    1.0f, -1.0f, -1.0f, // back-left
    -1.0f, -1.0f, -1.0f, // back-right

    // left
    0.0f,  1.0f,  0.0f, // left-top
    -1.0f, -1.0f, -1.0f, // left-left
    -1.0f, -1.0f,  1.0f, // left-right
}; 

const float pyramid_texCoords[] = 
{
    // front
    0.5, 1.0, // front-top
    0.0, 0.0, // front-left
    1.0, 0.0, // front-right

    // right
    0.5, 1.0, // right-top
    1.0, 0.0, // right-left
    0.0, 0.0, // right-right

    // back
    0.5, 1.0, // back-top
    0.0, 0.0, // back-left
    1.0, 0.0, // back-right

    // left
    0.5, 1.0, // left-top
    1.0, 0.0, // left-left
    0.0, 0.0, // left-right
}; 


#endif 
