#version 450 core

uniform sampler2D uSceneTexture;
uniform float uTime; // Animate grain over time
uniform float uGrainStrength;
uniform vec2 uResolution; // Screen resolution
uniform float uVignetteRadius;
uniform float uVignetteSoftness;

in vec2 fsTextureCoords;

out vec4 OutFragColor;

// Random noise generator using screen coordinates + time
float rand(vec2 co)
{
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233)) + uTime * 43758.5453) * 43758.5453);
}

void main()
{
    vec3 Color = texture(uSceneTexture, fsTextureCoords).rgb;

    // --- Film grain ---
    float Grain = rand(fsTextureCoords * uResolution) * uGrainStrength;
    Color += Grain;

    // --- Vignette ---
    vec2 Centered = fsTextureCoords - vec2(0.5f, 0.5f);
    float Dist = length(Centered);
    float Vignette = smoothstep(uVignetteRadius, uVignetteRadius - uVignetteSoftness, Dist);
    Color *= Vignette;

    OutFragColor = vec4(Color, 1.0f);
}