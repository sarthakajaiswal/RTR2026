#version 460 core

in vec4 aPosition;
in vec4 aColor;

out vec4 out_color;

uniform float uOffset; 
uniform mat4 uMVPMatrix;

void main(void) 
{
    gl_Position = uMVPMatrix * (aPosition + vec4(uOffset, 0.0f, 0.0f, 0.0f));
    out_color = aColor;
}

