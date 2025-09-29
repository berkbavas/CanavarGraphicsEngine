#version 450 core

layout(location = 0) in vec3 aPosition;

uniform mat4 uMVP;
uniform float uFarPlane;

out float fsFlogZ;

void main()
{
    gl_Position = uMVP * vec4(aPosition, 1.0f);

    float Coef = 2.0 / log2(uFarPlane + 1.0);
    gl_Position.z = log2(max(1e-6, 1.0 + gl_Position.w)) * Coef - 1.0;
    fsFlogZ = 1.0 + gl_Position.w;
}
