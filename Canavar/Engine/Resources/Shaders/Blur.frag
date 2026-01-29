#version 450 core

uniform sampler2D uTexture;
uniform bool uHorizontal;
uniform float uBlurScale;

in vec2 fsTextureCoords;

out vec4 OutFragColor;

// Gaussian weights for 9-tap blur
const float weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    vec2 texelSize = 1.0 / textureSize(uTexture, 0) * uBlurScale;
    vec3 result = texture(uTexture, fsTextureCoords).rgb * weights[0];

    if (uHorizontal)
    {
        for (int i = 1; i < 5; ++i)
        {
            result += texture(uTexture, fsTextureCoords + vec2(texelSize.x * i, 0.0)).rgb * weights[i];
            result += texture(uTexture, fsTextureCoords - vec2(texelSize.x * i, 0.0)).rgb * weights[i];
        }
    }
    else
    {
        for (int i = 1; i < 5; ++i)
        {
            result += texture(uTexture, fsTextureCoords + vec2(0.0, texelSize.y * i)).rgb * weights[i];
            result += texture(uTexture, fsTextureCoords - vec2(0.0, texelSize.y * i)).rgb * weights[i];
        }
    }

    OutFragColor = vec4(result, 1.0);
}
