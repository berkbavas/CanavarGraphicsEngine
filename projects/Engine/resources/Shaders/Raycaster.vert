#version 330 core
layout(location = 0) in vec3 position;

uniform mat4 M; // Model matrix
uniform mat4 VP; // View-Projection matrix

out vec4 fsPosition;

void main()
{
    fsPosition = M * vec4(position, 1);
    gl_Position = VP * fsPosition;
}
