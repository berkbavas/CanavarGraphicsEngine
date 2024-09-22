#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 textureCoords;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in int[4] ids;
layout(location = 6) in float[4] weights;

uniform mat4 M;  // Model matrix
uniform mat3 N;  // Normal matrix
uniform mat4 VP; // View-Projection matrix

uniform bool useTextureNormal;

out vec4 fsPosition;
out vec3 fsNormal;
out vec2 fsTextureCoords;
out mat3 fsTBN;

void main()
{
    fsPosition = M * vec4(position, 1.0);
    fsNormal = N * normal;
    fsTextureCoords = textureCoords;

    if (useTextureNormal)
    {
        vec3 T3 = normalize(vec3(M * vec4(tangent, 0.0)));
        vec3 B3 = normalize(vec3(M * vec4(bitangent, 0.0)));
        vec3 N3 = normalize(vec3(M * vec4(normal, 0.0)));
        fsTBN = N * mat3(T3, B3, N3);
    }

    gl_Position = VP * fsPosition;
}
