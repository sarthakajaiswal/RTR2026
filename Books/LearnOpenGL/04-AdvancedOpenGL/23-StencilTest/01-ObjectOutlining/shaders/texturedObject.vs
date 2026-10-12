#version 460 core 

in vec3 aPosition; 
in vec2 aTexCoords0; 

uniform mat4 u_mvpMatrix; 

out vec2 vs_texCoords; 

void main(void) 
{
    vs_texCoords = aTexCoords0; 
    gl_Position = u_mvpMatrix * vec4(aPosition, 1.0f); 
} 
