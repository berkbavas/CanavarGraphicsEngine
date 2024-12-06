#version 430 core

uniform vec4 color;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 brightColor;
layout(location = 2) out vec4 fragLocalPosition;
layout(location = 3) out vec4 fragWorldPosition;
layout(location = 4) out vec4 nodeInfo;

void main()
{
    outColor = color;
    brightColor = vec4(0);
}
