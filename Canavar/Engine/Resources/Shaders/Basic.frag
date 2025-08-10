#version 430 core

uniform vec4 uColor;

layout(location = 0) out vec4 OutColor;
layout(location = 1) out vec4 OutFragLocalPosition;
layout(location = 2) out vec4 OutFragWorldPosition;
layout(location = 3) out vec4 OutNodeInfo;
layout(location = 4) out vec4 OutFragVelocity;

void main()
{
    OutColor = uColor;
    OutFragVelocity = vec4(0.0f, 0.0f, 0.0f, 1.0f); // Placeholder for velocity output
}
