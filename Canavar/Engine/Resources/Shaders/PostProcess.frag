#version 430 core

uniform sampler2D uColorTexture;
uniform sampler2D uDistanceTexture;

uniform float uBlurThreshold;
uniform int uMaxSamples;

layout(location = 0) in vec2 fsTextureCoords;

layout(location = 0) out vec4 OutFragColor;

void main()
{
    vec4 distance4d = texture(uDistanceTexture, fsTextureCoords);
    float distance = distance4d.r;

    if (distance <= uBlurThreshold)
    {
        OutFragColor = texture(uColorTexture, fsTextureCoords);
    }
    else if (distance > uBlurThreshold || 0.5f > distance4d.a /*Sky*/)
    {
        int samples = int(distance / pow(uBlurThreshold, 1.05));

        if (samples > uMaxSamples)
        {
            samples = uMaxSamples;
        }

        vec2 texelSize = 1.0 / textureSize(uColorTexture, 0);

        vec4 total = vec4(0);

        for (int x = -samples; x <= samples; ++x)
        {
            for (int y = -samples; y <= samples; ++y)
            {
                total += texture(uColorTexture, fsTextureCoords + vec2(x, y) * texelSize);
            }
        }

        total /= pow(2 * samples + 1, 2);

        OutFragColor = vec4(total.rgb, 1.0f);
    }
}
