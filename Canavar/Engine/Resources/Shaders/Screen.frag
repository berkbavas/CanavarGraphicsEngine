#version 450 core

uniform sampler2D uColorTexture;

in vec2 fsTextureCoords;

out vec4 oFragColor;

void main()
{
    const vec3 Color = texture(uColorTexture, fsTextureCoords).rgb;
    oFragColor = vec4(clamp(Color, 0.0f, 1.0f), 1.0f);
}
