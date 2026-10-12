#pragma once 

#include <Windows.h> 
#include <stdio.h> 

#include <gl/glew.h> 
#include <gl/gl.h> 

#include "vmath.h" 

#define OFFSET_OF(type, mem)	((unsigned long long)&(((type*)0)->mem))

enum 
{
	AMC_ATTRIBUTE_POSITION = 0, 
	AMC_ATTRIBUTE_COLOR, 
	AMC_ATTRIBUTE_TEXTURE0, 
	AMC_ATTRIBUTE_TEXTURE1, 
	AMC_ATTRIBUTE_TEXTURE2,
	AMC_ATTRIBUTE_NORMAL,  
	AMC_ATTRIBUTE_TANGENT, 
	AMC_ATTRIBUTE_BITANGENT
}; 

struct Vertex 
{
	vmath::vec3 position; 
	vmath::vec2 texCoord; 
	vmath::vec3 normal; 
	vmath::vec3 tangent; 
	vmath::vec3 bitangent; 
}; 

struct E_camera;

extern FILE* gpFile; 

extern int gWinWidth, gWinHeight; 

void resize(int width, int height); 

