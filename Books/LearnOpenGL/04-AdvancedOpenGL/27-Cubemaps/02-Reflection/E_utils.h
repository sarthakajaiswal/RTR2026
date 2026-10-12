#pragma once

#include <Windows.h>
#include <gl/glew.h>
#include <iostream> 
#include <vector> 
#include "vmath.h" 

char* readFileIntoString(const char* filePath);
void loadTextureUsingSTBI(GLuint* textureID, const char* fileName);
void loadCubemapUsingSTBI(GLuint* textureID, const char** Images);  
void deleteTexture(GLuint* textureID); 
FLOAT GetElapsedTime(void);

// spline related 
float lerp(float t1, float t2, float k); 
float mapRange(float t1, float t2, float t); 
vmath::vec3 lerpVec3(vmath::vec3 p1, vmath::vec3 p2, float t); 
vmath::vec3 catmullRom4points(vmath::vec3 P0, vmath::vec3 P1, vmath::vec3 P2, vmath::vec3 P3, float t); 
vmath::vec3 bezier(vmath::vec3 A, vmath::vec3 B, vmath::vec3 C, float t); 
vmath::vec3 catmullRom(const std::vector<vmath::vec3>& positions, float t); 
float catmullRom(const std::vector<float>& values, float t); 
