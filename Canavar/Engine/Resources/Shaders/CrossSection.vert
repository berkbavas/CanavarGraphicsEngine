#version 450 core

layout(location = 0) in vec3 aPosition;

uniform mat4 uModelMatrix;

void main()
{
    gl_Position = uModelMatrix * vec4(aPosition, 1.0f);
}
