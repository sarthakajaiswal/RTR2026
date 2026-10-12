#version 460 core 

in vec3 aPosition; 

uniform mat4 u_mvpMatrix;  
out vec3 vs_localPos; 

void main(void) 
{
    vs_localPos = aPosition; 
    gl_Position = u_mvpMatrix * vec4(aPosition, 1.0f); 
} 
