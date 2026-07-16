#version 450 core

uniform sampler2D uSceneTexture;
uniform float uStrength; // Separation amount in UV space (default 0.003)

in vec2 fsTextureCoords;

out vec4 OutFragColor;

// Chromatic Aberration
// Separates R, G, B channels by progressively larger offsets from screen centre,
// giving the classic lens-fringing effect. Offset direction is radially outward.
void main()
{
    vec2 UV = fsTextureCoords;
    vec2 Center = vec2(0.5f);

    // Direction from centre, scaled by distance (stronger at edges)
    vec2 Dir = (UV - Center) * uStrength;

    float R = texture(uSceneTexture, UV + Dir * 1.0f).r;
    float G = texture(uSceneTexture, UV).g;
    float B = texture(uSceneTexture, UV - Dir * 1.0f).b;

    OutFragColor = vec4(R, G, B, 1.0f);
}
