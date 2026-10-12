#version 460 core

in vec4 aPosition;
in vec2 aTexCoords0; 

out vec2 out_texCoords;

uniform mat4 uMVPMatrix;

void main(void) 
{
    gl_Position = uMVPMatrix * aPosition;
    out_texCoords = aTexCoords0;
}

