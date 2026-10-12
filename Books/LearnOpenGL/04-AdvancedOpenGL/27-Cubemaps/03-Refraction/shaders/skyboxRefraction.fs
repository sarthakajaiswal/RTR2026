#version 460 core 
 
in vec3 vs_normal; 
in vec3 vs_worldPos; 

out vec4 FragColor; 
 
uniform samplerCube u_texture;  
uniform vec3 u_viewPosition; 

uniform float ri_Air = 1.0; 
uniform float ri_Glass = 1.52; 

void main(void) 
{
    float ratio     = ri_Air / ri_Glass; 
    vec3 viewDir    = normalize(vs_worldPos - u_viewPosition); 
    vec3 refractDir = refract(viewDir, normalize(vs_normal), ratio); 

    FragColor = texture(u_texture, refractDir);
} 
