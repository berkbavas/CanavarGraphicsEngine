#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 velocity;
layout(location = 2) in float life;

uniform mat4 MVP;

out vec3 fsPosition;

void main()
{
    fsPosition = position;
    gl_Position = MVP * vec4(position, 1.0f);
}
