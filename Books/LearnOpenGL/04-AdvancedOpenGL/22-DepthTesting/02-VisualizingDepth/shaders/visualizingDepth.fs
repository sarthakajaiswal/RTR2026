#version 460 core 

out vec4 FragColor; 

void main(void) 
{
    FragColor = vec4(vec3(gl_FragCoord.z), 1.0f); 
} 
