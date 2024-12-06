#version 430 core

layout(location = 0) in vec4 position;

uniform mat4 M;

void main()
{
    gl_Position = M * position;
}
