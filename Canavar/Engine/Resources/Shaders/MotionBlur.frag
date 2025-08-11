#version 430 core

uniform sampler2D uSceneTexture;    // HDR scene color
uniform sampler2D uVelocityTexture; // Screen-space velocity xy -> Velocity, z -> Depth
uniform float uStrength;            // controls shutter speed
uniform int uNumberOfSamples;
uniform float uZFar;
uniform float uZNear;
uniform float uDepthThreshold; // Max allowed depth difference

in vec2 fsTextureCoords;
out vec4 OutFragColor;

// Linearize depth (assumes perspective projection)
float LinearizeDepth(float depth, float near, float far)
{
    return (2.0 * near) / (far + near - depth * (far - near));
}

void main()
{
    vec2 velocity = texture(uVelocityTexture, fsTextureCoords).xy * uStrength;

    float speed = length(velocity);

    if (speed < 0.001f)
    {
        OutFragColor = texture(uSceneTexture, fsTextureCoords);
        return;
    }

    float depth = texture(uVelocityTexture, fsTextureCoords).z;
    float centerDepth = LinearizeDepth(depth, uZNear, uZFar);

    vec3 color = vec3(0.0);
    float totalWeight = 0.0;

    for (int i = 0; i < uNumberOfSamples; i++)
    {
        float t = float(i) / float(uNumberOfSamples - 1);
        vec2 sampleUV = fsTextureCoords + velocity * (t - 0.5);

        float sampleDepthRaw = texture(uVelocityTexture, sampleUV).z;
        float sampleDepth = LinearizeDepth(sampleDepthRaw, uZNear, uZFar);

        float depthDiff = abs(sampleDepth - centerDepth);

        if (depthDiff < uDepthThreshold)
        {
            float weight = 1.0 - (depthDiff / uDepthThreshold);
            vec3 sampleColor = texture(uSceneTexture, sampleUV).rgb;
            color += sampleColor * weight;
            totalWeight += weight;
        }
    }

    color /= max(totalWeight, 0.0001f); // Avoid division by zero

    OutFragColor = vec4(color, 1.0);
}