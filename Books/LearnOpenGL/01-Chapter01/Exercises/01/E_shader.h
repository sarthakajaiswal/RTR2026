#pragma once 

#include <gl/glew.h> 
#include <gl/GL.h>

GLuint createShaderFromSourceFiles(const char* vsPath, const char* fsPath); 
GLuint createShaderFromSourceStrings(const char* vsPath, const char* fsPath); 
void uninitShader(GLuint program); 

char* readFileIntoString(const char* filePath); 
