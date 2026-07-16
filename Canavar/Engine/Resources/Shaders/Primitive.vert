#version 430 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

uniform mat4 uModelMatrix;
uniform mat4 uVP;
uniform mat3 uNormalMatrix;
uniform float uFar;

out vec3 fsNormal;
out vec3 fsFragWorldPosition;
out float fsFlogZ;

void main()
{
    const vec4 WorldPos = uModelMatrix * vec4(aPosition, 1.0f);
    fsFragWorldPosition = WorldPos.xyz;
    fsNormal = normalize(uNormalMatrix * aNormal);

    gl_Position = uVP * WorldPos;

    const float Coef = 2.0f / log2(uFar + 1.0f);
    gl_Position.z = log2(max(1e-6f, 1.0f + gl_Position.w)) * Coef - 1.0f;
    fsFlogZ = 1.0f + gl_Position.w;
}
