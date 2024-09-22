#version 330 core

uniform float maxLife;

in vec3 fsPosition;
in float fsLife;
in float fsDeadAfter;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

void main()
{
    fragColor = vec4(1,1,1, pow((fsDeadAfter - fsLife) / fsDeadAfter, 8.0f));
    brightColor = fragColor;
}
