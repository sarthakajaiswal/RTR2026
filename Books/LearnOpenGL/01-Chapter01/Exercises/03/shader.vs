#version 460 core

in vec4 aPosition;

out vec4 out_position; 

uniform mat4 uMVPMatrix;

void main(void) 
{
    gl_Position = uMVPMatrix * aPosition;
    out_position = aPosition;
}

