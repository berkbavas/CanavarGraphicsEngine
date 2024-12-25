#version 430 core

uniform sampler2D colorTexture;
uniform sampler2D distanceTexture;

uniform float blurThreshold;
uniform int maxSamples;

layout(location = 0) in vec2 fsTextureCoords;

layout(location = 0) out vec4 outColor;

void main()
{
    vec4 distance4d = texture(distanceTexture, fsTextureCoords);
    float distance = distance4d.r;

    if (distance <= blurThreshold)
    {
        outColor = texture(colorTexture, fsTextureCoords);
    }
    else if(distance > blurThreshold || 0.5f > distance4d.a /*Sky*/)
    {
        int samples = int(distance / pow(blurThreshold, 1.05));

        if (samples > maxSamples)
        {
            samples = maxSamples;
        }

        vec2 texelSize = 1.0 / textureSize(colorTexture, 0);

        vec4 total = vec4(0);

        for (int x = -samples; x <= samples; ++x)
        {
            for (int y = -samples; y <= samples; ++y)
            {
                total += texture(colorTexture, fsTextureCoords + vec2(x, y) * texelSize);
            }
        }

        total /= pow(2 * samples + 1, 2);

        outColor = vec4(total.rgb, 1.0f);
    }
}
