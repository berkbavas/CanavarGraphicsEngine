#version 450 core

// ─────────────────────────────────────────────────────────────────────────────
//  GlobeTerrain.frag
// ─────────────────────────────────────────────────────────────────────────────

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vTexCoord;

uniform sampler2D uAlbedo;
uniform vec3  uSunDir;        // normalised direction TO the sun (ECEF-relative)
uniform vec3  uSunColor;      // sun light colour × intensity
uniform vec3  uAmbientColor;  // ambient light colour × intensity
uniform float uWireframe;     // 0.0 = solid, 1.0 = wireframe overlay

out vec4 FragColor;

void main()
{
    vec3  Albedo   = texture(uAlbedo, vTexCoord).rgb;
    vec3  N        = normalize(vNormal);
    float NdotL    = max(dot(N, normalize(uSunDir)), 0.0);
    vec3  Diffuse  = uSunColor * NdotL;
    vec3  Ambient  = uAmbientColor;

    vec3 Color = Albedo * (Ambient + Diffuse);

    if (uWireframe > 0.5)
        Color = mix(Color, vec3(0.0, 1.0, 0.4), 0.5);

    FragColor = vec4(Color, 1.0);
}
