#version 450 core

uniform sampler2D uBrightTexture; // Bright-pass result
uniform sampler2D uDepthTexture;  // Scene depth
uniform vec2 uScreenSpaceLightPosition;      // Light position in [0..1] screen coords
uniform mat4 uInverseProjection;  // Inverse projection matrix for depth test

uniform int uNumSamples = 100; // Number of samples for god rays
uniform float uDensity = 0.1; // Density of rays
uniform float uDecay = 0.95; // Decay of rays
uniform float uWeight = 0.1; // Weight of rays
uniform float uExposure = 0.5; // Exposure

in vec2 fsTextureCoords;
out vec4 OutFragColor;

float DepthAt(vec2 uv)
{
    float z = texture(uDepthTexture, uv).z;
    return z;
}

bool Occluded(vec2 uv, float lightDepth)
{
    float sceneDepth = DepthAt(uv);
    return sceneDepth < lightDepth - 0.01f;
}

float ReconstructViewZ(float depth)
{
    // Depth is in [0,1], convert to view space Z
    float ndc = depth * 2.0f - 1.0f;
    vec4 viewPos = uInverseProjection * vec4(0.0f, 0.0f, ndc, 1.0f);
    return viewPos.z / viewPos.w;
}

void main()
{
    vec2 delta = fsTextureCoords - uScreenSpaceLightPosition;
    delta *= uDensity / float(uNumSamples);

    vec2 coord = fsTextureCoords;
    vec4 color = vec4(0.0f);
    float lightDepth = ReconstructViewZ(DepthAt(uScreenSpaceLightPosition));

    float decayFactor = 1.0f;

    for (int i = 0; i < uNumSamples; i++)
    {
        coord -= delta;

        if (Occluded(coord, lightDepth))
        {
            break; // stop accumulating when blocked
        }

        vec4 samp = texture(uBrightTexture, coord);
        samp *= decayFactor * uWeight;
        color += samp;
        decayFactor *= uDecay;
    }

    OutFragColor = color * uExposure;
}