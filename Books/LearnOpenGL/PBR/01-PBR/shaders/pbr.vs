#version 460 core
in vec3 aPosition;
in vec3 aNormal;
in vec2 aTexCoords0;

out vec2 vs_texCoords;
out vec3 vs_worldPos;
out vec3 vs_normal;

uniform mat4 u_projectionMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_modelMatrix;

void main()
{
    vs_texCoords = aTexCoords0;
    vs_worldPos = vec3(u_modelMatrix * vec4(aPosition, 1.0));
    vs_normal = transpose(inverse(mat3(u_modelMatrix))) * aNormal;   

    gl_Position =  u_projectionMatrix * u_viewMatrix * vec4(vs_worldPos, 1.0);
}

