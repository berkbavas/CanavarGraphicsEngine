#version 430 core

in vec2 fsTextureCoords;

uniform sampler2D uSourceTexture;

out vec4 OutColor;

void main()
{
    OutColor = texture(uSourceTexture, fsTextureCoords);
}
