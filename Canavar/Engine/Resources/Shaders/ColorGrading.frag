#version 450 core

uniform sampler2D uSceneTexture;
uniform float uSaturation;
uniform float uContrast;
uniform float uBrightness;
uniform float uGamma;
uniform vec3 uColorTint;
uniform bool uChromaticAberrationEnabled;
uniform float uChromaticAberrationStrength;
uniform vec2 uResolution;

in vec2 fsTextureCoords;

out vec4 OutFragColor;

// Luminance for saturation adjustment
float Luminance(vec3 color)
{
    return dot(color, vec3(0.2126, 0.7152, 0.0722));
}

// Apply saturation
vec3 ApplySaturation(vec3 color, float saturation)
{
    float luma = Luminance(color);
    return mix(vec3(luma), color, saturation);
}

// Apply contrast with proper pivot
vec3 ApplyContrast(vec3 color, float contrast)
{
    return (color - 0.5) * contrast + 0.5;
}

// Apply brightness
vec3 ApplyBrightness(vec3 color, float brightness)
{
    return color + brightness;
}

// Apply gamma correction
vec3 ApplyGamma(vec3 color, float gamma)
{
    return pow(max(color, vec3(0.0)), vec3(1.0 / gamma));
}

// Chromatic aberration effect
vec3 ApplyChromaticAberration(vec2 uv, float strength)
{
    vec2 center = vec2(0.5);
    vec2 dir = uv - center;
    float dist = length(dir);
    
    vec2 offset = dir * dist * strength * 0.02;
    
    float r = texture(uSceneTexture, uv + offset).r;
    float g = texture(uSceneTexture, uv).g;
    float b = texture(uSceneTexture, uv - offset).b;
    
    return vec3(r, g, b);
}

void main()
{
    vec3 color;
    
    // Apply chromatic aberration if enabled
    if (uChromaticAberrationEnabled)
    {
        color = ApplyChromaticAberration(fsTextureCoords, uChromaticAberrationStrength);
    }
    else
    {
        color = texture(uSceneTexture, fsTextureCoords).rgb;
    }
    
    // Apply color grading chain
    color = ApplyBrightness(color, uBrightness);
    color = ApplyContrast(color, uContrast);
    color = ApplySaturation(color, uSaturation);
    color *= uColorTint;
    color = ApplyGamma(color, uGamma);
    
    // Clamp to valid range
    color = clamp(color, 0.0, 1.0);
    
    OutFragColor = vec4(color, 1.0);
}
