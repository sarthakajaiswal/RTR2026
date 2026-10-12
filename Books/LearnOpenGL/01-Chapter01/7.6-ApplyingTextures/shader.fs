#version 460 core

in vec2 out_texCoords; 
out vec4 FragColor; 
uniform sampler2D uTexture; 

void main(void)
{
    FragColor = texture(uTexture, out_texCoords);
}


