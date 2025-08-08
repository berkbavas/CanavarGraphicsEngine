#version 430 core

layout(location = 0) in vec3 aPosition;

uniform mat4 uMVP;
uniform float uZFar;

out float fsFlogZ;

void main()
{
    gl_Position = uMVP * vec4(aPosition, 1.0f);

    float coef = 2.0 / log2(uZFar + 1.0);
    gl_Position.z = log2(max(1e-6, 1.0 + gl_Position.w)) * coef - 1.0;
    fsFlogZ = 1.0 + gl_Position.w;
}
