#version 450 core

uniform sampler2D uSceneColor; // your lit HDR color buffer
uniform sampler2D uReflection; // SSR buffer: rgb=hitColor, a=mask
uniform float uIntensity;      // global strength (0..1)
uniform float uFireflyClamp;   // e.g., 10–50 (in HDR units), 0 disables

in vec2 fsTextureCoords;

layout(location = 0) out vec4 oColor;

// (optional) simple luminance
float Luma(vec3 c)
{
    return dot(c, vec3(0.2126, 0.7152, 0.0722));
}

void main()
{
    vec3 base = texture(uSceneColor, fsTextureCoords).rgb;
    vec4 refl = texture(uReflection, fsTextureCoords); // rgb, a=mask

    // Optional: clamp bright SSR to reduce fireflies (esp. with thin geometry)
    if (uFireflyClamp > 0.0)
    {
        float l = Luma(refl.rgb);
        if (l > uFireflyClamp)
            refl.rgb *= (uFireflyClamp / max(l, 1e-6));
    }

    // Energy-conserving mix: mask already includes Fresnel/roughness/AO
    float w = clamp(refl.a * uIntensity, 0.0, 1.0);
    vec3 outRGB = mix(base, refl.rgb, w);

    oColor = vec4(outRGB, 1.0);
}