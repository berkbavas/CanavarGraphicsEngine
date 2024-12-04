#version 430 core

uniform vec4 color;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 brightColor;

void main()
{
    fragColor = color;
    brightColor = vec4(0);
}
