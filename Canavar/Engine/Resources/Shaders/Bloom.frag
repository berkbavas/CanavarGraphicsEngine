#version 450 core

uniform sampler2D uSceneTexture;
uniform sampler2D uBloomTexture;
uniform float uBloomIntensity;
uniform float uBloomThreshold;
uniform bool uExtractBrightness;

in vec2 fsTextureCoords;

out vec4 OutFragColor;

// Luminance calculation using standard weights
float Luminance(vec3 color)
{
    return dot(color, vec3(0.2126, 0.7152, 0.0722));
}

// Soft threshold for smooth brightness extraction
vec3 SoftThreshold(vec3 color, float threshold)
{
    float brightness = Luminance(color);
    float soft = brightness - threshold + 0.5;
    soft = clamp(soft, 0.0, 1.0);
    soft = soft * soft * (3.0 - 2.0 * soft); // Smoothstep
    return color * soft;
}

void main()
{
    vec3 sceneColor = texture(uSceneTexture, fsTextureCoords).rgb;
    
    if (uExtractBrightness)
    {
        // Extract bright areas for bloom
        vec3 brightColor = SoftThreshold(sceneColor, uBloomThreshold);
        OutFragColor = vec4(brightColor, 1.0);
    }
    else
    {
        // Combine scene with bloom
        vec3 bloomColor = texture(uBloomTexture, fsTextureCoords).rgb;
        vec3 result = sceneColor + bloomColor * uBloomIntensity;
        OutFragColor = vec4(result, 1.0);
    }
}
