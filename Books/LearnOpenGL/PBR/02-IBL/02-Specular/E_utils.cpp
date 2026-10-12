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
	stbi_set_flip_vertically_on_load(false); 

	glGenTextures(1, textureID);
	unsigned char* data = stbi_load(fileName, &width, &height, &nrComponents, 0);
	if (data)
	{
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

void loadCubemapUsingSTBI(GLuint* textureID, const char** Images)
{
	//variable declaration
	int width, height, nrComponents;
    GLuint texture; 

	//code
	stbi_set_flip_vertically_on_load(false); 

	glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    for(int i = 0; i < 6; ++i) 
    {
        unsigned char* data = stbi_load(Images[i], &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data); 
        }
        else
        {
            fprintf(gpFile, "Texture failed to load %s\n", Images[i]);
            glDeleteTextures(1, &texture); 
            *textureID = 0; 
            return; 
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0); 

    *textureID = texture; 
}

void loadHDRTextureUsingSTBI(GLuint* hdrTextureID, const char* hdrFileName) 
{
    // variable declarations 
    unsigned int textureID = 0;
    int width, height, nrComponents;

	stbi_set_flip_vertically_on_load(true);
    float *data = stbi_loadf(hdrFileName, &width, &height, &nrComponents, 0);

    if (data)
    {
		glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, 
					GL_RGB, GL_FLOAT, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		stbi_image_free(data); 
		fprintf(gpFile, "HDR Texture loading succesfull: %s\n", hdrFileName); 

        *hdrTextureID = textureID; 
    }
    else
    {
        fprintf(gpFile, "HDR Texture failed to load at path: %s\n", hdrFileName); 
    }
} 

GLuint convertEquirectangularMapIntoCubemap(const char* hdrTexturePath) 
{
    // function declarations 
    GLuint createShaderFromSourceFiles(const char* vsPath, const char* fsPath);  
    void renderCube(void); 

    // variable declarations 
    GLuint captureFBO;                  // rendering to create cubemap is done in this FBO
    GLuint hdrEquirectangularTexture=0; // texture from input image | we are converting this into cubemap 
    GLuint textureCubemap;              // converted texture (output) 
    int prevViewport[4];                // for storing viewport and restoring it at last  
    GLint prevFBO = 0;

    // code 
    glGetIntegerv(GL_VIEWPORT, prevViewport);
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);
     
    loadHDRTextureUsingSTBI(&hdrEquirectangularTexture, hdrTexturePath); 
    if(hdrEquirectangularTexture == 0) 
    {
        fprintf(gpFile, "failed to create equirectangular map from %s\n", hdrTexturePath); 
        return (0); 
    } 

    // create a framebuffer for storing final texture 
    glGenFramebuffers(1, &captureFBO); 
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO); 
    
    // create a texture as color buffer for above fbo 

    glGenTextures(1, &textureCubemap); 
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureCubemap); 
    for(unsigned int i = 0; i < 6; ++i) 
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, NULL); 
    } 
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // set projections to render cube into above color buffer 
    vmath::mat4 captureProjection = vmath::perspective(90.0f, 1.0f, 0.1f, 10.0f); 
    vmath::mat4 captureViews[] = 
    {
        vmath::lookat(vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3( 1.0f, 0.0f, 0.0f), vmath::vec3(0.0f,-1.0f, 0.0f)), 
        vmath::lookat(vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3(-1.0f, 0.0f, 0.0f), vmath::vec3(0.0f,-1.0f, 0.0f)),
        vmath::lookat(vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3( 0.0f, 1.0f, 0.0f), vmath::vec3(0.0f, 0.0f, 1.0f)),
        vmath::lookat(vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3( 0.0f,-1.0f, 0.0f), vmath::vec3(0.0f, 0.0f,-1.0f)),
        vmath::lookat(vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3( 0.0f, 0.0f, 1.0f), vmath::vec3(0.0f,-1.0f, 0.0f)),
        vmath::lookat(vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3( 0.0f, 0.0f,-1.0f), vmath::vec3(0.0f,-1.0f, 0.0f))
    }; 

    // now render cube with above camera settings into our framebuffer 
    GLuint equirectangularToCubemapProgram = createShaderFromSourceFiles("shaders/test.vs", "shaders/test.fs"); 

    glUseProgram(equirectangularToCubemapProgram); 
    { 
        glActiveTexture(GL_TEXTURE0); 
        glBindTexture(GL_TEXTURE_2D, hdrEquirectangularTexture); 
        glUniform1i(glGetUniformLocation(equirectangularToCubemapProgram, "u_equirectangularMap"), 0); 

        glViewport(0, 0, 512, 512); 
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO); 
        for(unsigned int i = 0; i < 6; ++i) 
        {
            glUniformMatrix4fv(glGetUniformLocation(equirectangularToCubemapProgram, "u_mvpMatrix"), 1, GL_FALSE, 
                    captureProjection*captureViews[i]); 
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, textureCubemap, 0); 
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

            renderCube(); 
        } 
    } 
    glUseProgram(0); 

    glBindTexture(GL_TEXTURE_CUBE_MAP, textureCubemap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
    glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // free temporary resources 
    glDeleteFramebuffers(1, &captureFBO);
    glDeleteTextures(1, &hdrEquirectangularTexture);
    glDeleteProgram(equirectangularToCubemapProgram);

    return (textureCubemap); 
} 

GLuint createIrradianceMapFromCubemap(GLuint cubemapTexture) 
{
    // function declarations 
    GLuint createShaderFromSourceFiles(const char* vsPath, const char* fsPath);  
    void renderCube(void); 
    
    // variable declarations 
    GLuint irradianceFBO;
    GLuint irradianceMap;
    int prevViewport[4];
    GLint prevFBO = 0;

    // code 
    // 1. save current viewport and fbo 
    glGetIntegerv(GL_VIEWPORT, prevViewport);
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);

    // 2. Create the Framebuffer
    glGenFramebuffers(1, &irradianceFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, irradianceFBO);

    // 3. Create the Cubemap Texture (32x32)
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        // Allocate 32x32 HDR floating point texture for each face
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // glGenerateMipmap(GL_TEXTURE_CUBE_MAP); 

    // 4. Set up Projection & View matrices
    vmath::mat4 captureProjection = vmath::perspective(90.0f, 1.0f, 0.1f, 10.0f);
    vmath::mat4 captureViews[] = 
    {
        vmath::lookat(vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3( 1.0f, 0.0f, 0.0f), vmath::vec3(0.0f,-1.0f, 0.0f)),
        vmath::lookat(vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3(-1.0f, 0.0f, 0.0f), vmath::vec3(0.0f,-1.0f, 0.0f)),
        vmath::lookat(vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3( 0.0f, 1.0f, 0.0f), vmath::vec3(0.0f, 0.0f, 1.0f)),
        vmath::lookat(vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3( 0.0f,-1.0f, 0.0f), vmath::vec3(0.0f, 0.0f,-1.0f)),
        vmath::lookat(vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3( 0.0f, 0.0f, 1.0f), vmath::vec3(0.0f,-1.0f, 0.0f)),
        vmath::lookat(vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3( 0.0f, 0.0f,-1.0f), vmath::vec3(0.0f,-1.0f, 0.0f))
    };

    // 5. Load and use the irradiance convolution shader
    GLuint irradianceShader = createShaderFromSourceFiles("shaders/cubemapToIrradiance.vs", "shaders/cubemapToIrradiance.fs");
    if(irradianceShader == 0) 
    {
        fprintf(gpFile, "Failed to create irradiance shader program\n"); 
    } 

    glUseProgram(irradianceShader);

    // Bind input environment cubemap
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glUniform1i(glGetUniformLocation(irradianceShader, "u_cubemap"), 0);

    // Set viewport to target resolution (32x32)
    glViewport(0, 0, 32, 32);
    glBindFramebuffer(GL_FRAMEBUFFER, irradianceFBO);

    // 6. Render 6 faces
    for (unsigned int i = 0; i < 6; ++i)
    {
        glUniformMatrix4fv(glGetUniformLocation(irradianceShader, "u_mvpMatrix"), 1, GL_FALSE, captureProjection * captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderCube();
    }

    // generate mip maps 
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    // 7. Restore states & cleanup
    glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
    glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);
    glDeleteFramebuffers(1, &irradianceFBO);
    glDeleteProgram(irradianceShader);

    return (irradianceMap);
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

