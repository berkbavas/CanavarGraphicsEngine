#version 430 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTextureCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

// Uniforms
uniform mat4 uModelMatrix;
uniform mat4 uVP;
uniform mat3 uNormalMatrix;
uniform float uFar;

// Outputs to the fragment shader
out vec3 fsFragWorldPosition;
out vec3 fsFragLocalPosition;
out vec3 fsNormal;
out vec2 fsTextureCoords;
out mat3 fsTBN;
flat out int fsVertexId;
out float fsFlogZ;

void main()
{
    fsFragLocalPosition = aPosition;
    const vec4 WorldPos = uModelMatrix * vec4(aPosition, 1.0f);
    fsFragWorldPosition = WorldPos.xyz;

    fsTextureCoords = aTextureCoords;

    vec3 T = normalize(uNormalMatrix * aTangent);
    vec3 B = normalize(uNormalMatrix * aBitangent);
    vec3 N = normalize(uNormalMatrix * aNormal);
    fsTBN = mat3(T, B, N);
    fsNormal = N;

    gl_Position = uVP * WorldPos;
    fsVertexId = gl_VertexID;

    float Coef = 2.0f / log2(uFar + 1.0f);
    gl_Position.z = log2(max(1e-6f, 1.0f + gl_Position.w)) * Coef - 1.0f;
    fsFlogZ = 1.0f + gl_Position.w;
}
