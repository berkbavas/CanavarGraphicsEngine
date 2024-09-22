#version 430 core
in vec2 fsTextureCoords;

uniform sampler2D screenTexture;
uniform bool horizontal;


out vec4 outColor;

uniform float weight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    int width =  textureSize(screenTexture, 0).x;
    int height = textureSize(screenTexture, 0).y;

    vec2 coords = fsTextureCoords;
    vec4 totalColor = texture(screenTexture, coords) * weight[0];

    if (horizontal)
    {
        for (int i = 1; i < 5; i++)
        {
            totalColor += texture(screenTexture, coords + vec2(i, 0) / width) * weight[i];
            totalColor += texture(screenTexture, coords - vec2(i, 0) / width) * weight[i];
        }

        outColor = totalColor;

    } else
    {
        for (int i = 1; i < 5; i++)
        {
            totalColor += texture(screenTexture, coords + vec2(0, i) / height) * weight[i];
            totalColor += texture(screenTexture, coords - vec2(0, i) / height) * weight[i];
        }

        outColor = totalColor;
    }

}
