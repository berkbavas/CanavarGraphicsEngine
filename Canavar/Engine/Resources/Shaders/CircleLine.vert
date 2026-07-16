#version 430 core

layout(location = 0) in vec3 aPosition;

uniform mat4 uModelMatrix;
uniform mat4 uVP;
uniform float uFar;

out float gsFlogZ;

void main()
{
    const vec4 WorldPos = uModelMatrix * vec4(aPosition, 1.0f);
    gl_Position = uVP * WorldPos;

    const float Coef = 2.0f / log2(uFar + 1.0f);
    gl_Position.z = log2(max(1e-6f, 1.0f + gl_Position.w)) * Coef - 1.0f;
    gsFlogZ = 1.0f + gl_Position.w;
}
