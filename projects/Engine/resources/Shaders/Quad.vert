#version 330 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 textureCoords;

out vec2 fsTextureCoords;

void main()
{
    gl_Position = vec4(position, 0, 1);
    fsTextureCoords = textureCoords;
}
