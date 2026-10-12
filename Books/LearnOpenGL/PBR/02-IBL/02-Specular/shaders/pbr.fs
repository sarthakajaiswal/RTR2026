    #version 460 core 

    out vec4 FragColor; 

    in vec2 vs_texCoords;
    in vec3 vs_worldPos;
    in vec3 vs_normal;

    // material textures 
    uniform sampler2D u_albedoMap;
    uniform sampler2D u_normalMap;
    uniform sampler2D u_metallicMap;
    uniform sampler2D u_roughnessMap;
    uniform sampler2D u_aoMap;

    uniform samplerCube u_irradianceMap;

    // lights 
    uniform vec3 u_lightPositions[4]; 
    uniform vec3 u_lightColors[4]; 

    uniform vec3 u_camPos; 

    const float PI = 3.14159265359; 

    vec3 getNormalFromNormalMap(void) 
    {
        vec3 tangentNormal = texture(u_normalMap, vs_texCoords).xyz * 2.0 - 1.0;

        vec3 Q1  = dFdx(vs_worldPos);
        vec3 Q2  = dFdy(vs_worldPos);
        vec2 st1 = dFdx(vs_texCoords);
        vec2 st2 = dFdy(vs_texCoords);

        vec3 N   = normalize(vs_normal);
        vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
        vec3 B  = -normalize(cross(N, T));
        mat3 TBN = mat3(T, B, N);

        return (normalize(TBN * tangentNormal));
    }  

    float DistributionGGX(vec3 N, vec3 H, float roughness) 
    {
        float a = roughness * roughness; 
        float a2 = a * a; 
        float NdotH = max(dot(N, H), 0.0); 
        float NdotH2 = NdotH*NdotH; 

        float nom = a2; 
        float denom = (NdotH2 * (a2 - 1.0) + 1.0); 
        denom = PI * denom * denom; 

        return (nom / denom); 
    } 

    float GeometrySchlickGGX(float NdotV, float roughness) 
    {
        float r = (roughness + 1.0); 
        float k = (r*r) / 8.0; 

        float nom = NdotV; 
        float denom = NdotV * (1.0 - k) + k; 

        return (nom / denom); 
    } 

    float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) 
    {
        float NdotV = max(dot(N, V), 0.0); 
        float NdotL = max(dot(N, L), 0.0); 
        float ggx2 = GeometrySchlickGGX(NdotV, roughness); 
        float ggx1 = GeometrySchlickGGX(NdotL, roughness); 

        return (ggx1 * ggx2); 
    } 

    vec3 fresnelSchlick(float cosTheta, vec3 F0) 
    {
        return (F0 + (1.0 - F0) * pow(clamp(1.0-cosTheta, 0.0, 1.0), 5.0)); 
    } 

    void main(void) 
    {
        // sampling material parameters from textures 
        vec3 albedo     = pow(texture(u_albedoMap, vs_texCoords).rgb, vec3(2.2));
        float metallic  = texture(u_metallicMap, vs_texCoords).r;
        float roughness = texture(u_roughnessMap, vs_texCoords).r;
        float ao        = texture(u_aoMap, vs_texCoords).r;
        vec3 N          = getNormalFromNormalMap(); 

        vec3 V = normalize(u_camPos - vs_worldPos); 

        // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
        // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
        vec3 F0 = vec3(0.04); 
        F0 = mix(F0, albedo, metallic);

        // reflectance equation 
        vec3 Lo = vec3(0.0); 
        for(int i = 0; i < 4; ++i) 
        {
            // calculate per-light radiance 
            vec3 L = normalize(u_lightPositions[i] - vs_worldPos); 
            vec3 H = normalize(V + L); 
            float distance = length(u_lightPositions[i] - vs_worldPos); 
            float attenuation = 1.0f / (distance * distance); 
            vec3 radiance = u_lightColors[i] * attenuation; 

            // cook torrance brdf 
            float NDF   = DistributionGGX(N, H, roughness); 
            float G     = GeometrySmith(N, V, L, roughness); 
            vec3 F      = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0); 

            vec3 numerator = NDF * G * F; 
            float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; 
            vec3 specular = numerator / denominator; 

            vec3 kS = F; 
            vec3 kD = vec3(1.0) - kS; 
            kD *= 1.0 - metallic; 

            // scale light by NdotL 
            float NdotL = max(dot(N, L), 0.0); 

            // add to outgoing radiance Lo 
            Lo += (kD * albedo / PI + specular) * radiance * NdotL; 
        } 

        vec3 kS = fresnelSchlick(max(dot(N, V), 0.0), F0);
        vec3 kD = 1.0 - kS;
        kD *= 1.0 - metallic;	  
        
        vec3 irradiance = texture(u_irradianceMap, N).rgb;
        vec3 diffuse    = irradiance * albedo;
        // vec3 ambient = vec3(0.03) * albedo * ao; 
        vec3 ambient    = kD * diffuse * ao;

        vec3 color = ambient + Lo; 

        // HDR tonemapping 
        color = color / (color + vec3(1.0)); 
        // gamma correct 
        color = pow(color, vec3(1.0/2.2)); 

        FragColor = vec4(color, 1.0); 
    } 
