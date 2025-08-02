#version 430 core

layout(location = 0) in vec3 position;

uniform mat4 M;

void main()
{
    gl_Position = M * vec4(position, 1.0f);
}
