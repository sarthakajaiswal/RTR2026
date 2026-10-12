#version 460 core 

struct Material 
{
    sampler2D diffuseTex; 
    sampler2D specularTex; 
    float   shininess; 
}; 

struct Light 
{
    vec3 position;
    vec3 direction; 

    vec3 ambient; 
    vec3 diffuse; 
    vec3 specular; 

    // for spotlight
    float cutoff;    
    float outerCutoff; 

    // for attenuation 
    float constantAttenuationFactor; 
    float linearAttenuationFactor; 
    float quadraticAttenuationFactor; 
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
    vec3 finalColor = vec3(0.0); 

    vec3 norm = normalize(vs_normal); 
    vec3 v3_fragmentToLight = normalize(u_light.position - vs_fragPosition);

    // ambient  
    vec3 materialDiffuse = texture(u_material.diffuseTex, vs_texCoord).rgb; 
    vec3 ambient = u_light.ambient * materialDiffuse; 

    // diffuse
    float diff = max(dot(norm, v3_fragmentToLight), 0.0f);    
    vec3 diffuse = u_light.diffuse * diff * materialDiffuse; 

    // specular 
    vec3 viewDirection = normalize(u_viewPosition - vs_fragPosition); 
    vec3 reflectDirection = reflect(-v3_fragmentToLight, norm); 
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0f), u_material.shininess); 
    vec3 materialSpecular = texture(u_material.specularTex, vs_texCoord).rgb; 
    vec3 specular = u_light.specular * spec * materialSpecular; 

    // spotlight 
    float theta = dot(v3_fragmentToLight, normalize(-u_light.direction)); 
    float epsilon = (u_light.cutoff - u_light.outerCutoff);
    float intensity = clamp((theta - u_light.outerCutoff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;

    float distance = length(u_light.position - vs_fragPosition); 
    // attenuation = 1.0f/ (kc + kl*d + kq*d*d)  
    float attenuation = 1.0f / (u_light.constantAttenuationFactor + (u_light.linearAttenuationFactor*distance) + (u_light.quadraticAttenuationFactor*distance*distance)); 
    diffuse *= attenuation; 
    specular *= attenuation; 

    finalColor = (ambient + diffuse + specular);  
    FragColor = vec4(finalColor, 1.0f);
} 
