#version 450 core

// ─────────────────────────────────────────────────────────────────────────────
//  GlobeTerrain.vert
//  Vertex positions are ECEF metres relative to the tile centre.
//  uTileCenterOffset = (tile_center_ecef − camera_ecef_origin), updated per tile.
// ─────────────────────────────────────────────────────────────────────────────

layout(location = 0) in vec3 aPosition;  // ECEF relative to tile centre
layout(location = 1) in vec3 aNormal;    // Surface normal (ECEF)
layout(location = 2) in vec2 aTexCoord;  // U=west→east, V=south→north

uniform mat4 uProjection;
uniform mat4 uView;
uniform vec3 uTileCenterOffset; // tile_centre_ecef − camera_ecef_origin  (per tile)

out vec3 vWorldPos;   // position relative to camera origin (for lighting)
out vec3 vNormal;
out vec2 vTexCoord;

void main()
{
    // World position relative to camera origin
    vWorldPos  = aPosition + uTileCenterOffset;
    vNormal    = aNormal;
    vTexCoord  = vec2(aTexCoord.x, 1.0 - aTexCoord.y); // flip V for GL texture convention

    gl_Position = uProjection * uView * vec4(vWorldPos, 1.0);
}
