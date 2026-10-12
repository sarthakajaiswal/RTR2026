#version 460 core 

in vec3 aPosition; 
in vec2 aTexCoords0; 

out vec3 vs_color; 

uniform mat4 u_mvpMatrix;  
uniform sampler2D u_heightmapTex;
uniform float u_heightScale; 

void main(void) 
{
    float height = texture(u_heightmapTex, aTexCoords0).r; 
    vs_color = vec3(height); 
    height = height * u_heightScale; 
    gl_Position = u_mvpMatrix * vec4(aPosition, 1.0f); 
} 
