#version 430 core

uniform sampler2D sceneTexture;
uniform sampler2D bloomTexture;

layout(location = 0) in vec2 fsTextureCoords;

layout(location = 0) out vec4 outColor;

void main()
{
    vec3 sceneColor = texture(sceneTexture, fsTextureCoords).rgb;
    vec3 bloomColor = texture(bloomTexture, fsTextureCoords).rgb;

    // Additive blending
    vec3 result = sceneColor + bloomColor;
    outColor = vec4(result, 1.0f);
}
