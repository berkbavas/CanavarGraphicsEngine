#version 450 core

uniform sampler2D uSceneTexture;
uniform vec2 uTexelSize; // 1.0 / resolution
uniform float uStrength; // 0.0 = no sharpening, 1.0 = full (default 0.5)

in vec2 fsTextureCoords;

out vec4 OutFragColor;

// Unsharp-mask sharpening
// Computes: original + strength * (original - blurred)
void main()
{
    vec2 UV = fsTextureCoords;

    // 3×3 box blur
    vec3 Blurred = vec3(0.0f);
    for (int Y = -1; Y <= 1; ++Y)
    {
        for (int X = -1; X <= 1; ++X)
        {
            Blurred += texture(uSceneTexture, UV + vec2(X, Y) * uTexelSize).rgb;
        }
    }
    Blurred /= 9.0f;

    vec3 Original = texture(uSceneTexture, UV).rgb;
    vec3 Sharpened = Original + uStrength * (Original - Blurred);

    OutFragColor = vec4(clamp(Sharpened, 0.0f, 1.0f), 1.0f);
}
