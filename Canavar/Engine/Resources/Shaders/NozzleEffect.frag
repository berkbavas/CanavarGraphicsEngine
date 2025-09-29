#version 450 core

uniform float uMaxRadius;
uniform float uFarPlane;

in vec3 fsPosition;
in float fsFlogZ;

layout(location = 0) out vec4 OutFragColor;

void main()
{
    float Distance = length(fsPosition);
    float Alpha = 1.0 - smoothstep(0.0, uMaxRadius, Distance);
    OutFragColor = vec4(1.0, 1.0, 1.0, Alpha);

    gl_FragDepth = log2(fsFlogZ) / log2(uFarPlane + 1.0);
}
