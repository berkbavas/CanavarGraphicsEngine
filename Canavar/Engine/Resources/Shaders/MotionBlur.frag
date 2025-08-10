#version 430 core

uniform sampler2D uSceneTexture;    // HDR scene color
uniform sampler2D uVelocityTexture; // Screen-space velocity
uniform float uStrength;       // controls shutter speed
uniform int uNumberOfSamples;

in vec2 fsTextureCoords;
out vec4 OutFragColor;

void main()
{
    vec3 color = vec3(0.0f);
    vec2 velocity = texture(uVelocityTexture, fsTextureCoords).xy;
    velocity *= uStrength; // scale by desired blur amount

    for (int i = 0; i < uNumberOfSamples; i++)
    {
        float t = float(i) / float(uNumberOfSamples - 1.0f);
        vec2 sampleUV = fsTextureCoords + velocity * (t - 0.5f);
        color += texture(uSceneTexture, sampleUV).rgb;
    }

    color /= float(uNumberOfSamples);
    OutFragColor = vec4(color, 1.0f);
}