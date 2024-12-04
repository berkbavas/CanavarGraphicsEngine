#version 430 core

layout(location = 0) in vec4 position;

uniform mat4 LVP; // Light View Projection Matrix
uniform mat4 M;   // Model Matrix

void main()
{
    gl_Position = LVP * M * vec4(position.xyz, 1.0);
}
