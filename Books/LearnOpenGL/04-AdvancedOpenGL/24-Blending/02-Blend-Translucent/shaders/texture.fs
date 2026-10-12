#version 460 core 

uniform sampler2D u_texture; 

in vec2 vs_texCoords; 
out vec4 FragColor; 

void main(void) 
{
    FragColor = texture(u_texture, vs_texCoords); 
} 
