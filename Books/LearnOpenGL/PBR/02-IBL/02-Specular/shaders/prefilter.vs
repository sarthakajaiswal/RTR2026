#version 460 core 

in vec3 aPosition; 
 
uniform mat4 u_viewMatrix; 
uniform mat4 u_projectionMatrix; 

out vec3 vs_worldPos; 

void main(void) 
{
    vs_worldPos = aPosition;
    gl_Position = u_projectionMatrix * u_viewMatrix * vec4(aPosition, 1.0f); 
} 


