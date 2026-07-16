#version 450 core

uniform sampler2D uSceneTexture;

uniform float uBrightness;  // additive offset  (default  0.0)
uniform float uContrast;    // multiplicative   (default  1.0)
uniform float uSaturation;  // 0 = greyscale, 1 = unchanged  (default 1.0)
uniform float uTemperature; // -1 cool (blue) .. +1 warm (orange)  (default 0.0)

in vec2 fsTextureCoords;

out vec4 OutFragColor;

// ─── RGB <-> HSV ──────────────────────────────────────────────────────────
vec3 RgbToHsv(vec3 c)
{
    vec4 K = vec4(0.0f, -1.0f / 3.0f, 2.0f / 3.0f, -1.0f);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10f;
    return vec3(abs(q.z + (q.w - q.y) / (6.0f * d + e)), d / (q.x + e), q.x);
}

vec3 HsvToRgb(vec3 c)
{
    vec4 K = vec4(1.0f, 2.0f / 3.0f, 1.0f / 3.0f, 3.0f);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0f - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0f, 1.0f), c.y);
}

void main()
{
    vec3 Color = texture(uSceneTexture, fsTextureCoords).rgb;

    // --- Brightness ---
    Color += uBrightness;

    // --- Contrast ---
    Color = (Color - 0.5f) * uContrast + 0.5f;

    // --- Saturation ---
    float Grey = dot(Color, vec3(0.299f, 0.587f, 0.114f));
    Color = mix(vec3(Grey), Color, uSaturation);

    // --- Color Temperature ---
    // Warm (+) shifts toward orange/red; cool (-) shifts toward blue
    Color.r += uTemperature * 0.1f;
    Color.g += uTemperature * 0.02f;
    Color.b -= uTemperature * 0.1f;

    Color = clamp(Color, 0.0f, 1.0f);

    OutFragColor = vec4(Color, 1.0f);
}
