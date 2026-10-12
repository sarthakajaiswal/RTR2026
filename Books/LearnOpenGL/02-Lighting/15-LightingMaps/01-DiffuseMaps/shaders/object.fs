#version 460 core 

struct Material 
{
    sampler2D diffuseTex; 
    vec3    specular; 
    float   shininess; 
}; 

struct Light 
{
    vec3 position; 
    vec3 ambient; 
    vec3 diffuse; 
    vec3 specular; 
    float shininess; 
}; 

uniform Material u_material; 
uniform Light u_light; 

uniform vec3 u_viewPosition; 

in vec3 vs_fragPosition; 
in vec3 vs_normal; 
in vec2 vs_texCoord; 

out vec4 FragColor; 

void main(void) 
{
    // ambient  
    vec3 materialDiffuse = texture(u_material.diffuseTex, vs_texCoord).rgb; 
    vec3 ambient = u_light.ambient * materialDiffuse; 
 
    vec3 norm = normalize(vs_normal); 
    vec3 lightDir = normalize(u_light.position - vs_fragPosition); 

    // diffuse
    float diff = max(dot(norm, lightDir), 0.0f);    
    vec3 diffuse = u_light.diffuse * diff * materialDiffuse; 

    // specular 
    vec3 viewDirection = normalize(u_viewPosition - vs_fragPosition); 
    vec3 reflectDirection = reflect(-lightDir, norm); 
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0f), u_material.shininess); 
    vec3 specular = u_light.specular * (spec * u_material.specular); 

    vec3 finalColor = ambient + diffuse + specular;  
    FragColor = vec4(finalColor, 1.0f); 
} 
