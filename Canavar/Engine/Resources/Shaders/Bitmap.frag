#version 430 core

in vec2 fsTextureCoords;

uniform sampler2D sourceTexture;

out vec4 outColor;

void main()
{
    outColor = texture(sourceTexture, fsTextureCoords);
}
