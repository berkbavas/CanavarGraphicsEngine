#version 430 core

uniform vec4 uColor;
uniform float uZFar;

in float fsFlogZ;

layout(location = 0) out vec4 OutFragColor;

void main()
{
    OutFragColor = uColor;
    gl_FragDepth = log2(fsFlogZ) / log2(uZFar + 1.0);
}
