#pragma once 

#include <gl/glew.h> 

GLuint createShaderFromSourceFiles(const char* vsPath, const char* fsPath); 
GLuint createShaderFromSourceStrings(const char* vsPath, const char* fsPath); 
void uninitShader(GLuint program); 

