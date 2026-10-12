#version 460 core 

uniform sampler2D u_texture; 

in vec2 vs_texCoords; 
out vec4 FragColor; 

void main(void) 
{
    vec4 texColor = texture(u_texture, vs_texCoords); 
    if(texColor.a < 0.1f) 
        discard; 
    FragColor = texColor; 
} 
