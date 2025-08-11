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
    vec3 color = texture(uSceneTexture, fsTextureCoords).rgb;

    // --- Film grain ---
    float grain = rand(fsTextureCoords * uResolution) * uGrainStrength;
    color += grain;

    // --- Vignette ---
    vec2 centered = fsTextureCoords - vec2(0.5f, 0.5f);
    float dist = length(centered);
    float vignette = smoothstep(uVignetteRadius, uVignetteRadius - uVignetteSoftness, dist);
    color *= vignette;

    OutFragColor = vec4(color, 1.0f);
}