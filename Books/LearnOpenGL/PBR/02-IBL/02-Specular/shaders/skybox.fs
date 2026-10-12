#version 460 core 

uniform samplerCube u_cubemap; 

in vec3 vs_texCoords; 
out vec4 FragColor;

void main(void) 
{
    vec3 texColor = texture(u_cubemap, vs_texCoords).rgb; 

    texColor = texColor/(texColor+vec3(1.0));
    texColor = pow(texColor, vec3(1.0/2.2));

    FragColor = vec4(texColor, 1.0f); 
} 
