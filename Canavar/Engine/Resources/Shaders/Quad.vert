#version 430 core

layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec2 aTextureCoords;

out vec2 fsTextureCoords;

void main()
{
    fsTextureCoords = aTextureCoords;
    gl_Position = vec4(aPosition, 0.0f, 1.0f);
}
