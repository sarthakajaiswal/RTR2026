#version 460 core 

in vec4 aPosition; 
 
uniform mat4 u_viewMatrix; 
uniform mat4 u_projectionMatrix; 

out vec3 vs_texCoords; 

void main(void) 
{
    vs_texCoords = aPosition.xyz;
    vec4 pos = u_projectionMatrix*mat4(mat3(u_viewMatrix)) * aPosition; 
    gl_Position = pos.xyww; 
} 
