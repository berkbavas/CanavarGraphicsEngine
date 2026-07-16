#version 450 core

uniform sampler2D uSceneTexture;
uniform float uBarrel; // > 0 barrel,  < 0 pincushion  (default 0.15)
uniform float uZoom;   // Compensating zoom to hide black borders (default 0.9)

in vec2 fsTextureCoords;

out vec4 OutFragColor;

// Radial lens distortion (barrel / pincushion)
// r' = r * (1 + k * r^2)  where k = uBarrel
void main()
{
    // Map UV to [-1, +1]
    vec2 UV = fsTextureCoords * 2.0f - 1.0f;

    float R2 = dot(UV, UV);
    vec2 Distorted = UV * (1.0f + uBarrel * R2);

    // Apply compensating zoom and remap back to [0, 1]
    vec2 SampledUV = Distorted * uZoom * 0.5f + 0.5f;

    // Clamp — pixels outside the frame are black
    if (SampledUV.x < 0.0f || SampledUV.x > 1.0f || SampledUV.y < 0.0f || SampledUV.y > 1.0f)
    {
        OutFragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
        return;
    }

    OutFragColor = vec4(texture(uSceneTexture, SampledUV).rgb, 1.0f);
}
