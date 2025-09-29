#version 450 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aVelocity;
layout(location = 2) in float aLife;

uniform mat4 uMVP;
uniform float uFarPlane;

out vec3 fsPosition;
out float fsFlogZ;

void main()
{
    fsPosition = aPosition;
    gl_Position = uMVP * vec4(aPosition, 1.0f);

    float Coef = 2.0 / log2(uFarPlane + 1.0);
    gl_Position.z = log2(max(1e-6, 1.0 + gl_Position.w)) * Coef - 1.0;
    fsFlogZ = 1.0 + gl_Position.w;
}
