#version 430 core

uniform sampler2D targetTexture;
uniform bool horizontal;

layout(location = 0) in vec2 fsTextureCoords;

layout(location = 0) out vec4 outColor;

uniform float weight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    int width = textureSize(targetTexture, 0).x;
    int height = textureSize(targetTexture, 0).y;

    vec2 coords = fsTextureCoords;
    vec4 color = texture(targetTexture, coords) * weight[0];

    if (horizontal)
    {
        for (int i = 1; i < 5; i++)
        {
            color += texture(targetTexture, coords + vec2(i, 0) / width) * weight[i];
            color += texture(targetTexture, coords - vec2(i, 0) / width) * weight[i];
        }

        outColor = vec4(color.rgb, 1.0f);
    }
    else
    {
        for (int i = 1; i < 5; i++)
        {
            color += texture(targetTexture, coords + vec2(0, i) / height) * weight[i];
            color += texture(targetTexture, coords - vec2(0, i) / height) * weight[i];
        }

        outColor = vec4(color.rgb, 1.0f);
    }
}
