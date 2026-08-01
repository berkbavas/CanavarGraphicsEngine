#version 450 core

uniform samplerCube uDepthMap;
uniform int uFaceIndex;

in vec2 fsTextureCoords;

layout(location = 0) out vec4 oFragColor;

// Map a texel UV (in [0,1]) plus a face index to a cubemap sampling direction.
vec3 CubeFaceDir(int Face, vec2 UV)
{
    // Convert to NDC
    float u = UV.x * 2.0f - 1.0f;
    float v = UV.y * 2.0f - 1.0f;

    if (Face == 0)
        return vec3(1.0f, -v, -u); // +X
    if (Face == 1)
        return vec3(-1.0f, -v, u); // -X
    if (Face == 2)
        return vec3(u, 1.0f, v); // +Y
    if (Face == 3)
        return vec3(u, -1.0f, -v); // -Y
    if (Face == 4)
        return vec3(u, -v, 1.0f); // +Z
    if (Face == 5)
        return vec3(-u, -v, -1.0f); // -Z

    return vec3(0.0f, 0.0f, 0.0f);
}

void main()
{
    float Depth = texture(uDepthMap, CubeFaceDir(uFaceIndex, fsTextureCoords)).r;
    oFragColor = vec4(vec3(Depth), 1.0);
}
