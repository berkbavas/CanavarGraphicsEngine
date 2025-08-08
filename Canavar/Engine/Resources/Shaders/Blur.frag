#version 430 core

uniform sampler2D uTargetTexture;
uniform bool uHorizontal;

layout(location = 0) in vec2 fsTextureCoords;

layout(location = 0) out vec4 OutColor;

uniform float weight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    int width = textureSize(uTargetTexture, 0).x;
    int height = textureSize(uTargetTexture, 0).y;

    vec2 coords = fsTextureCoords;
    vec4 color = texture(uTargetTexture, coords) * weight[0];

    if (uHorizontal)
    {
        for (int i = 1; i < 5; i++)
        {
            color += texture(uTargetTexture, coords + vec2(i, 0) / width) * weight[i];
            color += texture(uTargetTexture, coords - vec2(i, 0) / width) * weight[i];
        }

        OutColor = vec4(color.rgb, 1.0f);
    }
    else
    {
        for (int i = 1; i < 5; i++)
        {
            color += texture(uTargetTexture, coords + vec2(0, i) / height) * weight[i];
            color += texture(uTargetTexture, coords - vec2(0, i) / height) * weight[i];
        }

        OutColor = vec4(color.rgb, 1.0f);
    }
}
