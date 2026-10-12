#version 460 core 

uniform samplerCube u_cubemap; 

in vec3 vs_texCoords; 
out vec4 FragColor;

void main(void) 
{
    vec4 texColor = texture(u_cubemap, vs_texCoords); 
    FragColor = texColor; 
} 
