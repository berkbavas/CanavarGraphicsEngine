#version 430 core

layout(location = 0) in vec3 aPosition;

uniform mat4 uMVP;

void main()
{
    gl_Position = uMVP * vec4(aPosition, 1.0f);
}