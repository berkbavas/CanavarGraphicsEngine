#version 450 core

layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec2 aTextureCoords;

uniform mat4 uProjection;
uniform mat4 uTransformation;

out vec2 fsTextureCoords;

void main()
{
    fsTextureCoords = aTextureCoords;
    gl_Position = uProjection * uTransformation * vec4(aPosition, 0.0, 1.0);
}
