#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 velocity;
layout(location = 2) in float life;

uniform mat4 MVP;

uniform float zFar;

out vec3 fsPosition;
out float fsflogz;

void main()
{
    fsPosition = position;
    gl_Position = MVP * vec4(position, 1.0f);

    gl_Position.z = log2(max(1e-6, 1.0 + gl_Position.w)) * (2.0 / log2(zFar + 1.0)) - 1.0;

    fsflogz = 1.0 + gl_Position.w;
}
