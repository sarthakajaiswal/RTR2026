#version 460 core 

in vec3 aPosition; 
in vec2 aTexCoords0; 

uniform mat4 u_modelMatrix; 
uniform mat4 u_viewMatrix; 
uniform mat4 u_projectionMatrix; 

out vec2 vs_texCoords; 

void main(void) 
{
    vs_texCoords = aTexCoords0; 
    gl_Position = u_projectionMatrix*u_viewMatrix*u_modelMatrix * vec4(aPosition, 1.0f); 
} 
