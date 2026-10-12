#version 460 core 

in vec2 vs_texCoords; 
uniform sampler2D u_texture; 
out vec4 FragColor; 

void main(void) 
{
    FragColor = texture(u_texture, vs_texCoords); 
} 
