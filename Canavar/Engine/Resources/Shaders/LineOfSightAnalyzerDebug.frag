#version 450 core

uniform samplerCube uDepthMap;
uniform int uFaceIndex;

in vec2 fsTextureCoords;
out vec4 oFragColor;

// Map a texel UV (in [0,1]) plus a face index to a cubemap sampling direction.
vec3 CubeFaceDir(int Face, vec2 UV)
{
    // Convert to NDC and flip V so the image appears right-side up in ImGui.
    float u =        UV.x * 2.0 - 1.0;
    float v = 1.0 - (UV.y * 2.0 - 1.0);

    if (Face == 0) return vec3( 1.0, -v, -u); // +X
    if (Face == 1) return vec3(-1.0, -v,  u); // -X
    if (Face == 2) return vec3( u,  1.0,  v); // +Y
    if (Face == 3) return vec3( u, -1.0, -v); // -Y
    if (Face == 4) return vec3( u, -v,  1.0); // +Z
                   return vec3(-u, -v, -1.0); // -Z
}

void main()
{
    float Depth = texture(uDepthMap, CubeFaceDir(uFaceIndex, fsTextureCoords)).r;
    oFragColor = vec4(vec3(Depth), 1.0);
}
