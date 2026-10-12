#version 460 core 

in vec3 aPosition; 
in vec3 aNormal; 

uniform mat4 u_mvpMatrix;  
uniform mat4 u_modelMatrix;  

out vec3 vs_normal; 
out vec3 vs_fragPosition; 

void main(void) 
{
    vs_normal = mat3(transpose(inverse(u_modelMatrix))) * aNormal; 
    vs_fragPosition = vec3(u_modelMatrix * vec4(aPosition, 1.0f)); 

    gl_Position = u_mvpMatrix * vec4(aPosition, 1.0f); 
} 
