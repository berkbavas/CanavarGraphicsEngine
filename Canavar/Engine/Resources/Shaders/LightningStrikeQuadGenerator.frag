#version 450 core

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 brightColor;

in vec4 fsColor;

void main()
{
    fragColor = fsColor;
    brightColor = fsColor;
}