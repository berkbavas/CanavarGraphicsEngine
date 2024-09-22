#version 330 core

uniform sampler2D screenTexture;

in vec2 fsTextureCoords;

out vec4 outColor;

void main()
{
    outColor = texture(screenTexture, fsTextureCoords);
}
