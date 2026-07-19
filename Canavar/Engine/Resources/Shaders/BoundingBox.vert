#version 430 core

layout(location = 0) in vec3 aPosition;

uniform mat4 uVP;
uniform float uFar;

out float fsFlogZ;

void main()
{
    gl_Position = uVP * vec4(aPosition, 1.0f);

    const float Coef = 2.0f / log2(uFar + 1.0f);
    gl_Position.z = log2(max(1e-6f, 1.0f + gl_Position.w)) * Coef - 1.0f;
    fsFlogZ = 1.0f + gl_Position.w;
}
