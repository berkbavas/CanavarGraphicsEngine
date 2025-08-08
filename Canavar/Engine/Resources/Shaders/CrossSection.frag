#version 430 core

uniform vec4 uColor;

layout(location = 0) out vec4 OutColor;

void main()
{
    OutColor = uColor;
}
