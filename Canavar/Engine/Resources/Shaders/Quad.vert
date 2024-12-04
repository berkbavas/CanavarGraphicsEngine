#version 430 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 textureCoords;

layout(location = 0) out vec2 fsTextureCoords;

void main()
{
    fsTextureCoords = textureCoords;
    gl_Position = vec4(position, 0, 1);
}
