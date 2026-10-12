#version 460 core 

uniform vec3 u_lightColor; 
uniform vec3 u_objColor; 
uniform vec3 u_lightPosition;  
uniform vec3 u_viewPosition; 
uniform int u_specularPower; 

in vec3 vs_fragPosition; 
in vec3 vs_normal; 
out vec4 FragColor; 

void main(void) 
{
    // ambient 
    float ambientStrength = 0.1f; 
    vec3 ambient = ambientStrength * u_lightColor; 
 
    vec3 norm = normalize(vs_normal); 
    vec3 lightDir = normalize(u_lightPosition - vs_fragPosition); 

    // diffuse
    float diff = max(dot(vs_normal, lightDir), 0.0f);    
    vec3 diffuse = u_lightColor * diff; 

    // specular 
    float specularStrength = 0.5f; 
    vec3 viewDirection = normalize(u_viewPosition - vs_fragPosition); 
    vec3 reflectDirection = reflect(-lightDir, norm); 
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0f), u_specularPower); 
    vec3 specular = specularStrength * spec * u_lightColor; 

    vec3 finalColor = u_objColor * (ambient + diffuse + specular);  
    FragColor = vec4(finalColor, 1.0f); 
} 
