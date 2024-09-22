#version 330 core
layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 brightColor;

uniform vec4 color = vec4(1);

void main()
{
    fragColor = color;
    brightColor = vec4(0);
}
