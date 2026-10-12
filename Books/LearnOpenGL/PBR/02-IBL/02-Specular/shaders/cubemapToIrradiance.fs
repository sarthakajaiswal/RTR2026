#version 330 core

in vec3     vs_localPos;
out vec4    FragColor;
uniform samplerCube u_cubemap;

const float PI = 3.14159265359;

void main(void)
{
    // the sample direction equals the hemisphere’s orientation
    vec3 normal = normalize(vs_localPos);
    vec3 irradiance = vec3(0.0);
    vec3 up = vec3(0.0,1.0, 0.0);
    vec3 right = cross(up,normal);
    
    up = cross(normal, right);

    float sampleDelta = 0.025;
    int nrSamples = 0;
    for(float phi = 0.0; phi <2.0 * PI; phi += sampleDelta)
    {
        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            //sphericaltocartesian(intangentspace)
            vec3 tangentSample = vec3(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));

            //tangentspace toworld
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;
            
            irradiance += texture(u_cubemap, sampleVec).rgb * cos(theta) * sin(theta);

            nrSamples++;
        }
    }

    irradiance = PI * irradiance * (1.0 /float(nrSamples));
    FragColor = vec4(irradiance, 1.0);
}