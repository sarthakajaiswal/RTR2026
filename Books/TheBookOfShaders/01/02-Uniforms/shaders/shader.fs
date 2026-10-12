#version 460 core 

#ifdef GL_ES 
precision mediump float; 
#endif 

uniform vec2 u_resolution; 
uniform vec2 u_mouse; 
uniform float u_time; 

out vec4 FragColor; 

void main(void) 
{
    FragColor = vec4(abs(sin(u_time)), 0.0f, 0.0f, 1.0f); 
} 

