#version 460 core  
 
out vec4 FragColor; 
out vec3 vs_color; 

void main(void) 
{
    FragColor = vec4(vs_color, 1.0f); 
} 
