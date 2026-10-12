#version 460 core 

uniform vec3 u_lightColor; 
uniform vec3 u_objColor; 

out vec4 FragColor; 

void main(void) 
{
    FragColor = vec4(u_objColor * u_lightColor, 1.0f); 
} 
