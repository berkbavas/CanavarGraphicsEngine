#version 330 core
layout (location = 0) out vec4 outWorldPosition;

in vec4 fsPosition;

void main()
{
    outWorldPosition = vec4(fsPosition.xyz, 1);
}
