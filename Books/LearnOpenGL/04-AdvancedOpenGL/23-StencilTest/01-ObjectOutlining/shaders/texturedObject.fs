#version 460 core 

in vec2 vs_texCoords; 

out vec4 FragColor; 

uniform sampler2D u_texture;  

void main(void) 
{
    FragColor = texture(u_texture, vs_texCoords);
} 
