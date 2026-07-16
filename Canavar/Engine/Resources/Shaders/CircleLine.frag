#version 430 core

in float fsFlogZ;
in float fsEdgeDist; // signed distance from the centre line in pixels

uniform vec3 uColor;
uniform float uOpacity;
uniform float uThickness;
uniform int uNodeId;
uniform float uFar;

layout(location = 0) out vec4 OutFragColor;
layout(location = 1) out vec4 OutFragLocalPosition;
layout(location = 2) out vec4 OutFragWorldPosition;
layout(location = 3) out vec4 OutNodeInfo;

void main()
{
    // Logarithmic depth
    const float Coef = 2.0f / log2(uFar + 1.0f);
    gl_FragDepth = log2(fsFlogZ) * Coef * 0.5f;

    // Smooth alpha fade toward the edges for anti-aliasing
    const float Dist = abs(fsEdgeDist);
    const float Edge = uThickness * 0.5f;
    const float Alpha = uOpacity * (1.0f - smoothstep(Edge - 1.0f, Edge, Dist));

    OutFragColor = vec4(uColor, Alpha);
    OutFragLocalPosition = vec4(0.0f);
    OutFragWorldPosition = vec4(0.0f);
    OutNodeInfo = vec4(float(uNodeId), 0.0f, 0.0f, 1.0f);
}
