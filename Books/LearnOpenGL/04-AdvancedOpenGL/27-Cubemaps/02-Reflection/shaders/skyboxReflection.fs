#version 460 core 
 
in vec3 vs_normal; 
in vec3 vs_worldPos; 

out vec4 FragColor; 
 
uniform samplerCube u_texture;  
uniform vec3 u_viewPosition; 

void main(void) 
{
    vec3 viewDir    = normalize(vs_worldPos - u_viewPosition); 
    vec3 reflectDir = reflect(viewDir, normalize(vs_normal)); 

    FragColor = texture(u_texture, reflectDir);
} 
