#version 430 core

layout(location = 0) in vec4 position;

uniform mat4 M;
uniform mat4 VP;

void main()
{
    gl_Position = (VP * M) * position;
}
