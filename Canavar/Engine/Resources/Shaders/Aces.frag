#version 450 core

uniform sampler2D uSceneTexture;
uniform float uExposure;     // Pre-exposure multiplier (default 1.0)

in vec2 fsTextureCoords;

out vec4 OutFragColor;

// ACES filmic tone mapping approximation by Krzysztof Narkowicz
// https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
vec3 AcesFilm(vec3 x)
{
    const float a = 2.51f;
    const float b = 0.03f;
    const float c = 2.43f;
    const float d = 0.59f;
    const float e = 0.14f;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0f, 1.0f);
}

void main()
{
    vec3 Color = texture(uSceneTexture, fsTextureCoords).rgb;

    // Apply exposure
    Color *= uExposure;

    // ACES tone mapping
    Color = AcesFilm(Color);

    // Gamma correction (linear -> sRGB approximation)
    Color = pow(Color, vec3(1.0f / 2.2f));

    OutFragColor = vec4(Color, 1.0f);
}
