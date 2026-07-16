#version 450 core

uniform sampler2D uSceneTexture;
uniform vec2 uTexelSize;         // 1.0 / resolution
uniform float uSubpixelQuality;  // 0.0 – 1.0,  default 0.75
uniform float uEdgeThreshold;    // minimum edge contrast, default 0.125
uniform float uEdgeThresholdMin; // darkest shadow threshold, default 0.0312

in vec2 fsTextureCoords;

out vec4 OutFragColor;

// ─── Luminance helper ──────────────────────────────────────────────────────
float Luma(vec3 rgb)
{
    return dot(rgb, vec3(0.299f, 0.587f, 0.114f));
}

// ─── FXAA 3.11 simplified implementation ──────────────────────────────────
// Reference: Timothy Lottes – FXAA white paper
void main()
{
    vec2 UV = fsTextureCoords;

    // --- Sample 3x3 neighbourhood ---
    vec3 rgbNW = textureOffset(uSceneTexture, UV, ivec2(-1, 1)).rgb;
    vec3 rgbNE = textureOffset(uSceneTexture, UV, ivec2(1, 1)).rgb;
    vec3 rgbSW = textureOffset(uSceneTexture, UV, ivec2(-1, -1)).rgb;
    vec3 rgbSE = textureOffset(uSceneTexture, UV, ivec2(1, -1)).rgb;
    vec3 rgbM = texture(uSceneTexture, UV).rgb;

    float lumaNW = Luma(rgbNW);
    float lumaNE = Luma(rgbNE);
    float lumaSW = Luma(rgbSW);
    float lumaSE = Luma(rgbSE);
    float lumaM = Luma(rgbM);

    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
    float lumaRange = lumaMax - lumaMin;

    // --- Early exit on low-contrast pixels ---
    if (lumaRange < max(uEdgeThresholdMin, lumaMax * uEdgeThreshold))
    {
        OutFragColor = vec4(rgbM, 1.0f);
        return;
    }

    // --- Compute blend direction ---
    vec2 Dir;
    Dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    Dir.y = ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    float DirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25f * uSubpixelQuality), 1.0f / 128.0f);
    float RcpDirMin = 1.0f / (min(abs(Dir.x), abs(Dir.y)) + DirReduce);
    Dir = clamp(Dir * RcpDirMin, vec2(-8.0f), vec2(8.0f)) * uTexelSize;

    // --- Sample along edge ---
    vec3 rgbA = 0.5f * (texture(uSceneTexture, UV + Dir * (1.0f / 3.0f - 0.5f)).rgb + texture(uSceneTexture, UV + Dir * (2.0f / 3.0f - 0.5f)).rgb);

    vec3 rgbB = rgbA * 0.5f + 0.25f * (texture(uSceneTexture, UV + Dir * -0.5f).rgb + texture(uSceneTexture, UV + Dir * 0.5f).rgb);

    float lumaB = Luma(rgbB);

    if ((lumaB < lumaMin) || (lumaB > lumaMax))
        OutFragColor = vec4(rgbA, 1.0f);
    else
        OutFragColor = vec4(rgbB, 1.0f);
}
