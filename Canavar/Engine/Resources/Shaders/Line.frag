#version 450 core

uniform vec4 color = vec4(1.0f);

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 brightColor;

void main()
{
    fragColor = color;
    brightColor = vec4(0);
}
