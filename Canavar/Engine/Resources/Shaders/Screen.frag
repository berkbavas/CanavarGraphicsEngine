#version 450 core

uniform sampler2D uColorTexture;

in vec2 fsTextureCoords;

out vec4 OutFragColor;

void main()
{
    OutFragColor = vec4(texture(uColorTexture, fsTextureCoords).rgb, 1.0f);
}