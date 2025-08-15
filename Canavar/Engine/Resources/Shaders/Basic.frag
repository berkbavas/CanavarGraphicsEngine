#version 450 core

uniform vec4 uColor;

layout(location = 0) out vec4 OutColor;
layout(location = 1) out vec4 OutFragLocalPosition;
layout(location = 2) out vec4 OutFragWorldPosition;
layout(location = 3) out vec4 OutNodeInfo;

void main()
{
    OutColor = uColor;
}
