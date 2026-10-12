#version 460 core 

in vec3 aPosition; 

uniform mat4 u_mvpMatrix;  

void main(void) 
{
    gl_Position = u_mvpMatrix * vec4(aPosition, 1.0f); 
} 
