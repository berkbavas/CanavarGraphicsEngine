#version 430 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 textureCoords;

uniform mat4 projection;
uniform mat4 transformation;

out vec2 fsTextureCoords;

void main()
{
    fsTextureCoords = textureCoords;
    gl_Position = projection * transformation * vec4(position, 0.0, 1.0);
}
