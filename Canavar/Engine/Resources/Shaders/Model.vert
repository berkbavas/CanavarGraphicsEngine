#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 textureCoords;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

uniform mat4 M;  // Model matrix
uniform mat3 N;  // Normal matrix
uniform mat4 VP; // View-Projection matrix

uniform float zFar;

out vec3 fsLocalPosition;
out vec3 fsWorldPosition;
out vec3 fsNormal;
out vec2 fsTextureCoords;
out mat3 fsTBN;
flat out int fsVertexId;
out float fsFlogZ;

void main()
{
    fsLocalPosition = position;
    vec4 worldPos = M * vec4(fsLocalPosition, 1.0f);
    fsWorldPosition = worldPos.xyz;
    fsTextureCoords = textureCoords.xy;

    vec3 T = normalize(N * tangent);
    vec3 B = normalize(N * bitangent);
    vec3 N = normalize(N * normal);
    fsTBN = mat3(T, B, N);
    fsNormal = normal;
    fsVertexId = gl_VertexID;
    gl_Position = VP * worldPos;

    float coef = 2.0 / log2(zFar + 1.0);
    gl_Position.z = log2(max(1e-6, 1.0 + gl_Position.w)) * coef - 1.0;
    fsFlogZ = 1.0 + gl_Position.w;
}
