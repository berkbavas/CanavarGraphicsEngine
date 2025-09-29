#version 450 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

uniform float uFarPlane;

uniform mat4 uViewProjectionMatrix;
uniform mat4 uModelMatrix;
uniform mat3 uNormalMatrix;
uniform mat4 uLightViewProjectionMatrix;    // Light View-Projection matrix

out vec3 fsLocalPosition;
out vec3 fsWorldPosition;
out vec3 fsNormal;
flat out int fsVertexId;
out float fsFlogZ;
out vec4 fsLightSpacePosition; // Position in light space

void main()
{
    fsLocalPosition = aPosition;
    vec4 WorldPos = uModelMatrix * vec4(fsLocalPosition, 1.0f);
    fsWorldPosition = WorldPos.xyz;
    fsLightSpacePosition = uLightViewProjectionMatrix * WorldPos;

    vec3 N = normalize(uNormalMatrix * aNormal);
    fsNormal = N;
    fsVertexId = gl_VertexID;

    gl_Position = uViewProjectionMatrix * WorldPos;

    float Coef = 2.0 / log2(uFarPlane + 1.0);
    gl_Position.z = log2(max(1e-6, 1.0 + gl_Position.w)) * Coef - 1.0;
    fsFlogZ = 1.0 + gl_Position.w;
}
