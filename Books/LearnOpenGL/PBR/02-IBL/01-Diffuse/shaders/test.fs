#version 460 core 

in vec3 vs_localPos; 
out vec4 FragColor; 
uniform sampler2D u_equirectangularMap; 

const vec2 invAtan = vec2(0.1591, 0.3183); 

vec2 SampleSphericalMap(vec3 v); 

void main(void) 
{
    vec2 uv = SampleSphericalMap(normalize(vs_localPos)); 
    FragColor = texture(u_equirectangularMap, uv);  
} 

vec2 SampleSphericalMap(vec3 v) 
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y)); 
    uv *= invAtan; 
    uv += 0.5; 
    return (uv); 
} 

