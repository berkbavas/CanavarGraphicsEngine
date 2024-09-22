#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 textureCoords;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in int[4] ids;
layout(location = 6) in float[4] weights;


uniform mat4 M; // Model matrix
uniform mat4 VP; // View-Projection matrix

out vec4 fsPosition;
out vec3 fsNormal;

void main()
{
    fsPosition = M * vec4(position, 1.0);
    fsNormal = normal;
    gl_Position = VP * fsPosition;
}
