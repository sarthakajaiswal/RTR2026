#version 460 core 

in vec4 aPosition; 
uniform mat4 u_mvpMatrix; 

void main(void) 
{
    gl_Position = u_mvpMatrix * aPosition; 
} 
