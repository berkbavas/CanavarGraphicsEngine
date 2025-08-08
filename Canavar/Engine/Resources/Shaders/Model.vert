#version 430 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTextureCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

uniform mat4 uModelMatrix;  // Model matrix
uniform mat3 uNormalMatrix;  // Normal matrix
uniform mat4 uViewProjectionMatrix; // View-Projection matrix

uniform float uZFar;

out vec3 fsLocalPosition;
out vec3 fsWorldPosition;
out vec3 fsNormal;
out vec2 fsTextureCoords;
out mat3 fsTBN;
flat out int fsVertexId;
out float fsFlogZ;

void main()
{
    fsLocalPosition = aPosition;
    vec4 worldPos = uModelMatrix * vec4(fsLocalPosition, 1.0f);
    fsWorldPosition = worldPos.xyz;
    fsTextureCoords = aTextureCoords.xy;

    vec3 T = normalize(uNormalMatrix * aTangent);
    vec3 B = normalize(uNormalMatrix * aBitangent);
    vec3 N = normalize(uNormalMatrix * aNormal);
    fsTBN = mat3(T, B, N);
    fsNormal = N;
    fsVertexId = gl_VertexID;
    gl_Position = uViewProjectionMatrix * worldPos;

    float coef = 2.0 / log2(uZFar + 1.0);
    gl_Position.z = log2(max(1e-6, 1.0 + gl_Position.w)) * coef - 1.0;
    fsFlogZ = 1.0 + gl_Position.w;
}
