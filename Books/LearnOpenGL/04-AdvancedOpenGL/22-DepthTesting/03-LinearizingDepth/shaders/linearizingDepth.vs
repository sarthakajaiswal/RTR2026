#version 460 core 

in vec3 aPosition; 

uniform mat4 u_modelMatrix; 
uniform mat4 u_viewMatrix; 
uniform mat4 u_projectionMatrix; 

void main(void) 
{
    gl_Position = u_projectionMatrix*u_viewMatrix*u_modelMatrix * vec4(aPosition, 1.0f); 
} 
