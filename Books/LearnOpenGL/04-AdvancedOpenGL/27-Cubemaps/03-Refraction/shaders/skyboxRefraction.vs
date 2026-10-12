#version 460 core 

in vec4 aPosition; 
in vec3 aNormal; 

uniform mat4 u_modelMatrix; 
uniform mat4 u_viewMatrix; 
uniform mat4 u_projectionMatrix; 

out vec3 vs_normal; 
out vec3 vs_worldPos; 

void main(void) 
{
    vs_normal   = mat3(transpose(inverse(u_modelMatrix))) * aNormal; 
    vs_worldPos = (u_modelMatrix * aPosition).xyz; 

    gl_Position = u_projectionMatrix * u_viewMatrix * aPosition; 
} 
