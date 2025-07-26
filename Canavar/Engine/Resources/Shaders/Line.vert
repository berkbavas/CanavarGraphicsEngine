#version 430 core

layout(location = 0) in vec3 position;

uniform mat4 MVP;
uniform float zFar;

out float fsFlogZ;

void main()
{
    gl_Position = MVP * vec4(position, 1.0f);

    float coef = 2.0 / log2(zFar + 1.0);
    gl_Position.z = log2(max(1e-6, 1.0 + gl_Position.w)) * coef - 1.0;
    fsFlogZ = 1.0 + gl_Position.w;
}
