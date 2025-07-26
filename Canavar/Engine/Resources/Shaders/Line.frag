#version 430 core

uniform vec4 color;
uniform float zFar;

in float fsFlogZ;

layout(location = 0) out vec4 fragColor;

void main()
{
    fragColor = color;
    gl_FragDepth = log2(fsFlogZ) / log2(zFar + 1.0);
}
