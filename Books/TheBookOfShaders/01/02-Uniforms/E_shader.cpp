#include <Windows.h> 
#include <stdio.h> 

#include <gl/glew.h> 
#include <gl/GL.h> 

#include "E_shader.h"
#include "E_utils.h" 
#include "globals.h" 

extern FILE* gpFile;  

GLuint createShaderFromSourceStrings(const char* vsCode, const char* fsCode) 
{
    GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER); 
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vsCode, NULL); 
	glCompileShader(vertexShaderObject); 

	GLint status = 0; 
	GLint infoLogLength = 0; 
	GLchar* szInfoLog = NULL; 
	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status); 
	if(status == GL_FALSE) 
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength); 
		if(infoLogLength > 0) 
		{
			szInfoLog = (GLchar*)malloc(infoLogLength * sizeof(GLchar)); 
			if(szInfoLog != NULL) 
			{
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, NULL, szInfoLog); 
				fprintf(gpFile, "vertex shader compilation log = %s\n", szInfoLog);  
			
				free(szInfoLog); 
				szInfoLog = NULL; 
			}
		}

		return (0);  
	} 

    // fragment shader 
	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER); 

	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fsCode, NULL); 
	glCompileShader(fragmentShaderObject); 

	status = 0; 
	infoLogLength = 0; 
	szInfoLog = NULL; 

	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status); 
	if(status == GL_FALSE) 
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength); 
		if(infoLogLength > 0) 
		{
			szInfoLog = (GLchar*)malloc(infoLogLength * sizeof(GLchar)); 
			if(szInfoLog != NULL) 
			{
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, NULL, szInfoLog); 
				fprintf(gpFile, "Fragment shader compilation log = %s\n", szInfoLog); 

				free(szInfoLog); 
				szInfoLog = NULL; 
			}
		}

		return (-2); 
	}

	GLuint program = glCreateProgram(); 

	glAttachShader(program, vertexShaderObject); 
    glBindAttribLocation(program, SJ_ATTRIBUTE_POSITION, "aPosition"); 
	glBindAttribLocation(program, SJ_ATTRIBUTE_COLOR, "aColor"); 
	glBindAttribLocation(program, SJ_ATTRIBUTE_TEXTURE0, "aTexCoord"); 
	glBindAttribLocation(program, SJ_ATTRIBUTE_NORMAL, "aNormal"); 
	glAttachShader(program, fragmentShaderObject); 

	glLinkProgram(program); 

	status = 0; 
	infoLogLength = 0; 
	szInfoLog = 0; 
	glGetProgramiv(program, GL_LINK_STATUS, &status); 
	if(status == GL_FALSE) 
	{
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength); 
		if(infoLogLength > 0) 
		{
			szInfoLog = (GLchar*)malloc(infoLogLength * sizeof(GLchar)); 
			if(szInfoLog != NULL) 
			{
				glGetProgramInfoLog(program, infoLogLength, NULL, szInfoLog); 
				fprintf(gpFile, "shader program object = %s\n", szInfoLog); 

				free(szInfoLog); 
				szInfoLog = NULL; 
			}
		}

		return (0); 
	} 

	return (program);
} 

GLuint createShaderFromSourceFiles(const char* vsPath, const char* fsPath) 
{
	// vertex shader 
    char* vertexShaderSourceCode = readFileIntoString(vsPath); 
    if(!vertexShaderSourceCode) 
    {
        fprintf(gpFile, "createShaderFromSourceFiles() > failed to read %s\n", vsPath); 
        return (0); 
    } 

    // fragment shader 
    char* fragmentShaderSourceCode = readFileIntoString(fsPath); 
    if(!fragmentShaderSourceCode) 
    {
        fprintf(gpFile, "createShaderFromSourceFiles() > failed to read %s\n", fsPath); 
        return (0); 
    } 

    GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER); 
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL); 
	glCompileShader(vertexShaderObject); 

	GLint status = 0; 
	GLint infoLogLength = 0; 
	GLchar* szInfoLog = NULL; 
	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status); 
	if(status == GL_FALSE) 
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength); 
		if(infoLogLength > 0) 
		{
			szInfoLog = (GLchar*)malloc(infoLogLength * sizeof(GLchar)); 
			if(szInfoLog != NULL) 
			{
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, NULL, szInfoLog); 
				fprintf(gpFile, "vertex shader compilation log = %s\n", szInfoLog);  
			
				free(szInfoLog); 
				szInfoLog = NULL; 
			}
		}

		return (0);  
	} 

    // fragment shader 
	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER); 

	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL); 
	glCompileShader(fragmentShaderObject); 

	status = 0; 
	infoLogLength = 0; 
	szInfoLog = NULL; 

	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status); 
	if(status == GL_FALSE) 
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength); 
		if(infoLogLength > 0) 
		{
			szInfoLog = (GLchar*)malloc(infoLogLength * sizeof(GLchar)); 
			if(szInfoLog != NULL) 
			{
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, NULL, szInfoLog); 
				fprintf(gpFile, "Fragment shader compilation log = %s\n", szInfoLog); 

				free(szInfoLog); 
				szInfoLog = NULL; 
			}
		}

		return (0); 
	}

	GLuint program = glCreateProgram(); 

	glAttachShader(program, vertexShaderObject); 
    glBindAttribLocation(program, SJ_ATTRIBUTE_POSITION, "aPosition"); 
	glBindAttribLocation(program, SJ_ATTRIBUTE_COLOR, "aColor"); 
	glBindAttribLocation(program, SJ_ATTRIBUTE_TEXTURE0, "aTexCoords0");
	glBindAttribLocation(program, SJ_ATTRIBUTE_TEXTURE1, "aTexCoords1");
	glBindAttribLocation(program, SJ_ATTRIBUTE_TEXTURE2, "aTexCoords2"); 
	glBindAttribLocation(program, SJ_ATTRIBUTE_NORMAL, "aNormal"); 
	glAttachShader(program, fragmentShaderObject); 

	glLinkProgram(program); 

	status = 0; 
	infoLogLength = 0; 
	szInfoLog = 0; 
	glGetProgramiv(program, GL_LINK_STATUS, &status); 
	if(status == GL_FALSE) 
	{
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength); 
		if(infoLogLength > 0) 
		{
			szInfoLog = (GLchar*)malloc(infoLogLength * sizeof(GLchar)); 
			if(szInfoLog != NULL) 
			{
				glGetProgramInfoLog(program, infoLogLength, NULL, szInfoLog); 
				fprintf(gpFile, "shader program object = %s\n", szInfoLog); 

				free(szInfoLog); 
				szInfoLog = NULL; 
			}
		}

		return (0); 
	} 

    fprintf(gpFile, "shader program created succesfully\n"); 

    glDetachShader(program, vertexShaderObject); 
    glDetachShader(program, fragmentShaderObject); 
    glDeleteShader(vertexShaderObject); 
    glDeleteShader(fragmentShaderObject); 

	return (program);
} 

void uninitShader(GLuint program) 
{
    if(program) 
    {
        glDeleteProgram(program); 
        fprintf(gpFile, "Shader program deleted succesfully\n"); 
    } 
} 
