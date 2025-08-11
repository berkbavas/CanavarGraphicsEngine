#version 450 core

uniform sampler2D uSceneTexture; // Input HDR texture
uniform float uExposure;         // Exposure value

vec3 ACESFilm(vec3 x)
{
    // Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0f, 1.0f);
}

in vec2 fsTextureCoords;

out vec4 OutFragColor;

void main()
{
    vec3 hdrColor = texture(uSceneTexture, fsTextureCoords).rgb;
    hdrColor *= uExposure; // Apply exposure
    vec3 ldrColor = ACESFilm(hdrColor);
    OutFragColor = vec4(ldrColor, 1.0f);
}