#version 450 core

// ─────────────────────────────────────────────────────────────────────────────
// Depth of Field post-process effect
//
// Uses the world-position G-buffer attachment to derive per-pixel depth,
// computes a Circle of Confusion (CoC) from the focus parameters, then
// accumulates a Poisson-disc blur whose radius is proportional to the CoC.
// ─────────────────────────────────────────────────────────────────────────────

uniform sampler2D uSceneTexture;         // HDR colour (Pong / input FBO)
uniform sampler2D uWorldPositionTexture; // World-space XYZ from G-buffer (attachment 2)

uniform vec3 uCameraPosition; // Camera world position
uniform float uFocusDistance; // Distance at which objects are perfectly sharp
uniform float uFocusRange;    // ± half-width of the in-focus zone (world units)
uniform float uMaxBlurRadius; // Maximum CoC blur radius in pixels
uniform vec2 uResolution;     // Render-target resolution (pixels)

in vec2 fsTextureCoords;
out vec4 OutFragColor;

// ─────────────────────────────────────────────────────────────────────────────
// 16-tap Poisson disc (normalised to unit circle)
// ─────────────────────────────────────────────────────────────────────────────
const int NUM_SAMPLES = 16;
const vec2 PoissonDisc[NUM_SAMPLES] = vec2[](vec2(-0.94201624, -0.39906216), //
                                             vec2(0.94558609, -0.76890725),
                                             vec2(-0.09418410, -0.92938870),
                                             vec2(0.34495938, 0.29387760),
                                             vec2(-0.91588581, 0.45771432),
                                             vec2(-0.81544232, -0.87912464),
                                             vec2(-0.38277543, 0.27676845),
                                             vec2(0.97484398, 0.75648379),
                                             vec2(0.44323325, -0.97511554),
                                             vec2(0.53742981, -0.47373420),
                                             vec2(-0.26496911, -0.41893023),
                                             vec2(0.79197514, 0.19090188),
                                             vec2(-0.24188840, 0.99706507),
                                             vec2(-0.81409955, 0.91437590),
                                             vec2(0.19984126, 0.78641367),
                                             vec2(0.14383161, -0.14100790));

// ─────────────────────────────────────────────────────────────────────────────
// Returns linear world-space depth for a given UV.
// Sky / unrendered pixels are cleared to (0,0,0,1); we identify them by their
// zero xyz magnitude and treat them as "at infinity".
// ─────────────────────────────────────────────────────────────────────────────
float SampleDepth(vec2 TexCoords)
{
    vec3 WorldPos = texture(uWorldPositionTexture, TexCoords).xyz;
    if (dot(WorldPos, WorldPos) < 1e-6)
    {
        return 1e8; // Sky / background → effectively at infinity
    }
    return length(WorldPos - uCameraPosition);
}

// ─────────────────────────────────────────────────────────────────────────────
// Maps a depth value to a Circle-of-Confusion radius in [0, 1].
//   0  →  perfectly in focus
//   1  →  fully blurred
//
// The in-focus zone is [FocusDistance - FocusRange, FocusDistance + FocusRange].
// CoC ramps linearly from 0 to 1 over a second FocusRange-wide transition band.
// ─────────────────────────────────────────────────────────────────────────────
float ComputeCoC(float Depth)
{
    float Dist = max(0.0, abs(Depth - uFocusDistance) - uFocusRange);
    return clamp(Dist / max(uFocusRange, 0.1), 0.0, 1.0);
}

// ─────────────────────────────────────────────────────────────────────────────
void main()
{
    float CenterDepth = SampleDepth(fsTextureCoords);
    float CenterCoC = ComputeCoC(CenterDepth);
    float BlurRadius = CenterCoC * uMaxBlurRadius;

    // Fast path: pixel is in focus – no blur needed
    if (BlurRadius < 0.5)
    {
        OutFragColor = texture(uSceneTexture, fsTextureCoords);
        return;
    }

    vec2 TexelSize = 1.0 / uResolution;
    vec4 AccumColor = texture(uSceneTexture, fsTextureCoords);
    float TotalWeight = 1.0;

    for (int i = 0; i < NUM_SAMPLES; ++i)
    {
        vec2 Offset = PoissonDisc[i] * BlurRadius * TexelSize;
        vec2 SampleUV = clamp(fsTextureCoords + Offset, vec2(0.0), vec2(1.0));

        // Allow background (larger CoC) samples to spread into this pixel,
        // but prevent sharp foreground pixels from bleeding into blurry areas.
        float SampleDepth = SampleDepth(SampleUV);
        float SampleCoC = ComputeCoC(SampleDepth);
        float SampleBlur = SampleCoC * uMaxBlurRadius;

        // Weight: a sample contributes fully when its own blur is at least as
        // large as the centre pixel's blur – prevents sharp-foreground bleed.
        float Weight = clamp(SampleBlur / max(BlurRadius, 1.0), 0.0, 1.0);

        AccumColor += texture(uSceneTexture, SampleUV) * Weight;
        TotalWeight += Weight;
    }

    OutFragColor = AccumColor / max(TotalWeight, 1e-4);
}
