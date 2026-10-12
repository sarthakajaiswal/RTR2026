#pragma once 

#include <Windows.h> 
#include <stdio.h> 

#include <gl/glew.h> 
#include <gl/gl.h> 

#include "vmath.h" 

#define OFFSET_OF(type, mem)	((unsigned long long)&(((type*)0)->mem))

enum 
{
	SJ_ATTRIBUTE_POSITION = 0, 
	SJ_ATTRIBUTE_COLOR, 
	SJ_ATTRIBUTE_TEXTURE0, 
	SJ_ATTRIBUTE_TEXTURE1, 
	SJ_ATTRIBUTE_TEXTURE2,
	SJ_ATTRIBUTE_NORMAL,  
	SJ_ATTRIBUTE_TANGENT, 
	SJ_ATTRIBUTE_BITANGENT
}; 

struct Vertex 
{
	vmath::vec3 position; 
	vmath::vec2 texCoord; 
	vmath::vec3 normal; 
	vmath::vec3 tangent; 
	vmath::vec3 bitangent; 
}; 

extern FILE* gpFile; 
extern int gWinWidth, gWinHeight; 

void resize(int width, int height); 

