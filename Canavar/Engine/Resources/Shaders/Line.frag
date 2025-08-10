#version 430 core

uniform vec4 uColor;
uniform float uZFar;

in float fsFlogZ;

layout(location = 0) out vec4 OutFragColor;
layout(location = 4) out vec4 OutFragVelocity;

void main()
{
    OutFragColor = uColor;
    OutFragVelocity = vec4(0.0f, 0.0f, 0.0f, 1.0f); // Placeholder for velocity output
    gl_FragDepth = log2(fsFlogZ) / log2(uZFar + 1.0);
}
