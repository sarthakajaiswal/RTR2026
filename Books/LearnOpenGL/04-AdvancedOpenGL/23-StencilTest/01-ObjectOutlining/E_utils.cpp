#include <Windows.h>
#include <stdio.h>
#include <iostream> 
#include <vector> 

#include <gl/glew.h>

#include "E_utils.h"
#include "vmath.h"
#include "stb_image.h"
#include "globals.h" 

char* readFileIntoString(const char* filePath)
{
    FILE* f = fopen(filePath, "rb"); 
    if(!f) 
    {
        printf("readFileIntoString() > cannot open %s\n", filePath); 
        return NULL; 
    } 

    fseek(f, 0, SEEK_END); 
    long len = ftell(f); 
    rewind(f); 

    char* buf = (char*)malloc(len + 1); 
    if(buf) 
    {
        fread(buf, 1, len, f); 
        buf[len] = '\0'; 
    } 

    fclose(f);
    return buf;
}

void loadTextureUsingSTBI(GLuint* textureID, const char* fileName)
{
	//variable declaration
	int width, height, nrComponents;

	//code
	stbi_set_flip_vertically_on_load(true); 

	unsigned char* data = stbi_load(fileName, &width, &height, &nrComponents, 0);
	if (data)
	{
        glGenTextures(1, textureID);

		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;
		glBindTexture(GL_TEXTURE_2D, *textureID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glGenerateTextureMipmap(*textureID);
		glBindTexture(GL_TEXTURE_2D, NULL);
	}
	else
	{
		fprintf(gpFile, "Texture failed to load %s\n", fileName);
	}
}

void deleteTexture(GLuint* textureID) 
{
    if(textureID) 
        glDeleteTextures(1, textureID);  

    *textureID = 0; 
} 

FLOAT GetElapsedTime(void)
{
    static ULONGLONG startTick = 0;
    ULONGLONG currentTick = GetTickCount64();

    if (startTick == 0)
    {
        startTick = currentTick;
    }

    return (FLOAT)(currentTick - startTick) / 1000.0f;
}

/////////////////////////////////////////////////////////////////////////////////////
// SPLINE // 
///////////////////////////////////////////////////////////////////////////////////// 

float lerp(float t1, float t2, float k)
{
    return k * t2 + (1.0f - k) * t1;
}

float mapRange(float t1, float t2, float t)
{
    if (t1 == t2) return t1;
    return (t - t1) / (t2 - t1);
}

vmath::vec3 lerpVec3(vmath::vec3 p1, vmath::vec3 p2, float t)
{
    vmath::vec3 ans;
    ans[0] = lerp(p1[0], p2[0], t);
    ans[1] = lerp(p1[1], p2[1], t);
    ans[2] = lerp(p1[2], p2[2], t);

    return ans;
}

vmath::vec3 catmullRom4points(vmath::vec3 P0, vmath::vec3 P1, vmath::vec3 P2, vmath::vec3 P3, float t)
{
    vmath::vec3 ans;
    
    // Using 0.5f float literals to avoid double-precision conversions
    ans[0] = 0.5f * ((2.0f * P1[0]) + (-P0[0] + P2[0]) * t
        + (2.0f * P0[0] - 5.0f * P1[0] + 4.0f * P2[0] - P3[0]) * t * t
        + (-P0[0] + 3.0f * P1[0] - 3.0f * P2[0] + P3[0]) * t * t * t);

    ans[1] = 0.5f * ((2.0f * P1[1]) + (-P0[1] + P2[1]) * t
        + (2.0f * P0[1] - 5.0f * P1[1] + 4.0f * P2[1] - P3[1]) * t * t
        + (-P0[1] + 3.0f * P1[1] - 3.0f * P2[1] + P3[1]) * t * t * t);

    ans[2] = 0.5f * ((2.0f * P1[2]) + (-P0[2] + P2[2]) * t
        + (2.0f * P0[2] - 5.0f * P1[2] + 4.0f * P2[2] - P3[2]) * t * t
        + (-P0[2] + 3.0f * P1[2] - 3.0f * P2[2] + P3[2]) * t * t * t);

    return ans;
}

vmath::vec3 bezier(vmath::vec3 A, vmath::vec3 B, vmath::vec3 C, float t)
{
    vmath::vec3 AB = lerpVec3(A, B, t);
    vmath::vec3 BC = lerpVec3(B, C, t);
    vmath::vec3 ABBC = lerpVec3(AB, BC, t);

    return ABBC;
}

vmath::vec3 catmullRom(const std::vector<vmath::vec3>& positions, float t)
{
    if (positions.size() < 4) 
    {
        if (positions.empty()) 
            return vmath::vec3(0.0f); 
        else 
            return positions[0]; 
    } 

    // Clamp t strictly in [0.0, 1.0] 
    t = std::max(0.0f, std::min(1.0f, t)); 

    int numSegments = static_cast<int>(positions.size()) - 3; 
    float scaledT = t * numSegments; 
    int currentSegment = static_cast<int>(scaledT); 
    
    // Safety check to prevent indexing past positions.size() - 4
    if (currentSegment >= numSegments)
    {
        currentSegment = numSegments - 1;
        scaledT = static_cast<float>(numSegments);
    }

    float localT = scaledT - static_cast<float>(currentSegment); 

    return catmullRom4points(
        positions[currentSegment], 
        positions[currentSegment + 1], 
        positions[currentSegment + 2], 
        positions[currentSegment + 3], 
        localT
    ); 
}

float catmullRom4floats(float p0, float p1, float p2, float p3, float t)
{
    float t2 = t * t;
    float t3 = t2 * t;

    return 0.5f * (
        (2.0f * p1) +
        (-p0 + p2) * t +
        (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
        (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3
    );
}

float catmullRom(const std::vector<float>& values, float t)
{
    if (values.size() < 4) 
    {
        if (values.empty()) 
            return 0.0f; 
        else 
            return values[0]; 
    } 

    // Clamp t strictly in [0.0, 1.0] 
    t = std::max(0.0f, std::min(1.0f, t)); 

    int numSegments = static_cast<int>(values.size()) - 3; 
    float scaledT = t * numSegments; 
    int currentSegment = static_cast<int>(scaledT); 
    
    // Safety check to prevent indexing out of bounds
    if (currentSegment >= numSegments)
    {
        currentSegment = numSegments - 1;
        scaledT = static_cast<float>(numSegments);
    }

    float localT = scaledT - static_cast<float>(currentSegment); 

    return catmullRom4floats(
        values[currentSegment], 
        values[currentSegment + 1], 
        values[currentSegment + 2], 
        values[currentSegment + 3], 
        localT
    ); 
}

