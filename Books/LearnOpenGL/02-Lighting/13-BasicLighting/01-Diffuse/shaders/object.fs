#version 460 core 

uniform vec3 u_lightColor; 
uniform vec3 u_objColor; 
uniform vec3 u_lightPosition;  

in vec3 vs_fragPosition; 
in vec3 vs_normal; 
out vec4 FragColor; 

void main(void) 
{
    vec3 norm = normalize(vs_normal); 
    vec3 lightDir = normalize(u_lightPosition - vs_fragPosition); 
    float diff = max(dot(vs_normal, lightDir), 0.0f); 
    
    vec3 diffuse = u_lightColor * diff; 

    FragColor = vec4(u_objColor * diffuse, 1.0f); 
} 
