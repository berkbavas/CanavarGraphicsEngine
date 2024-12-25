#version 430 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;
layout(location = 2) in vec4 textureCoords;
layout(location = 3) in vec4 tangent;
layout(location = 4) in vec4 bitangent;
layout(location = 5) in vec4 color;

uniform mat4 M;   // Model matrix
uniform mat3 N;   // Normal matrix
uniform mat4 VP;  // View-projection matrix
uniform mat4 LVP; // Light view-projection matrix

uniform float zFar;

uniform bool useTextureNormal;

out vec4 fsLocalPosition;
out vec4 fsWorldPosition;
out vec3 fsNormal;
out vec2 fsTextureCoords;
out mat3 fsTBN;
out vec4 fsFragPosLightSpace;
out vec4 fsVertexColor;
flat out int fsVertexId;
out float fsflogz;

void main()
{
    fsLocalPosition = vec4(position.xyz, 1.0f);
    fsWorldPosition = M * fsLocalPosition;

    fsTextureCoords = textureCoords.xy;

    vec3 T3 = normalize(N * tangent.xyz);
    vec3 N3 = normalize(N * normal.xyz);
    T3 = normalize(T3 - dot(T3, N3) * N3);
    vec3 B3 = cross(N3, T3);

    fsTBN = transpose(mat3(T3, B3, N3));

    fsNormal = N3;

    fsFragPosLightSpace = LVP * vec4(fsWorldPosition.xyz, 1.0f);

    fsVertexColor = color;

    fsVertexId = gl_VertexID;

    gl_Position = VP * fsWorldPosition;

    gl_Position.z = log2(max(1e-6, 1.0 + gl_Position.w)) * (2.0 / log2(zFar + 1.0)) - 1.0;

    fsflogz = 1.0 + gl_Position.w;
}
