#version 460 core

in vec4 out_position; 
out vec4 FragColor; 

void main(void)
{
    FragColor = out_position;
}


