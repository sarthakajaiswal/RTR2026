#version 460 core 

out vec4 FragColor; 

uniform float u_far = 100.0f; 
uniform float u_near = 0.1f; 

float LinearizeDepth(float normalizedDepth) 
{
    float z = normalizedDepth * 2.0f - 1.0f; 
    float linearDepth = (2.0f * u_near * u_far) / (u_far + u_near - z * (u_far - u_near)); 

    return (linearDepth); 
} 

void main(void) 
{
    float depth = LinearizeDepth(gl_FragCoord.z) / u_far; // dividing by far so that it comes under 1.0 (to show it in color, as above 1.0 will always white)  
    FragColor = vec4(vec3(depth), 1.0f);
} 
