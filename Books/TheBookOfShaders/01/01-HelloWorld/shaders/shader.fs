#version 460 core 

#ifdef GL_ES 
precision mediump float; 
#endif 

out vec4 FragColor; 

void main(void) 
{
    FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f); 
} 

