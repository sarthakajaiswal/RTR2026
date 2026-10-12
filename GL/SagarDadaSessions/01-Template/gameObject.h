#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <gl/glew.h> 	// this header file must be included before gl/GL.h 
#include <gl/GL.h> 

#include "vmath.h" 
#include "values.h" 
#include "camera.h" 

extern GLuint defaultShaderProgram; 
extern struct Camera engineCam; 

enum 
{
	AMC_ATTRIBUTE_POSITION = 0,  
	AMC_ATTRIBUTE_TEXCOORD, // (1) 
}; 

struct GameObject 
{
    GLuint VAO; 
    GLuint vbo_position; 
    GLuint vbo_texCoord; 
    GLuint shader; 
    vmath::mat4 modelMat; 
}; 

GLuint createVAOfromBuffers() 
{
    GLuint VAO; 
    GLuint vbo_position_pyramid; 
    GLuint vbo_texCoord_pyramid; 

	glGenVertexArrays(1, &VAO); 
	glBindVertexArray(VAO); 
	
	// position  
	glGenBuffers(1, &vbo_position_pyramid); 
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_pyramid); 
		glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid_position), pyramid_position, GL_STATIC_DRAW); 
		glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL); 
		glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION); 
	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	// texcoord 
	glGenBuffers(1, &vbo_texCoord_pyramid); 
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texCoord_pyramid); 
		glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid_texCoords), pyramid_texCoords, GL_STATIC_DRAW); 
		glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL); 
		glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD); 
	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	glBindVertexArray(0); 

    return (VAO); 
} 

struct GameObject createGameObject(const char* name) 
{
    struct GameObject gameObj; 

    gameObj.shader = defaultShaderProgram; 
    gameObj.modelMat = vmath::mat4::identity(); 
    gameObj.VAO = createVAOfromBuffers(); 

    return (gameObj); 
} 

void renderGameObj(GameObject gameObj) 
{
    glUseProgram(gameObj.shader); 

	vmath::mat4 modelViewProjectionMatrix = vmath::mat4::identity(); 
	modelViewProjectionMatrix = engineCam.projectionMat * engineCam.viewMat * gameObj.modelMat;  
	
	glUniformMatrix4fv(glGetUniformLocation(gameObj.shader, "uMVPMatrix"), 1, GL_FALSE, modelViewProjectionMatrix);  

    glBindVertexArray(gameObj.VAO); 
	glDrawArrays(GL_TRIANGLES, 0, 12); 
	glBindVertexArray(0); 

    glUseProgram(0); 
} 

