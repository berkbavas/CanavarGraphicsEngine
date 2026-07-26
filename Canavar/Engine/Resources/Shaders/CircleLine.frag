#version 430 core

in float fsFlogZ;
in float fsEdgeDist; // signed distance from the centre line in pixels

uniform vec3 uColor;
uniform float uOpacity;
uniform float uThickness;
uniform int uNodeId;
uniform float uFar;

layout(location = 0) out vec4 oFragColor;
layout(location = 1) out vec4 oFragLocalPosition;
layout(location = 2) out vec4 oFragWorldPosition;
layout(location = 3) out vec4 oNodeInfo;

void main()
{
    // Logarithmic depth
    const float Coef = 2.0f / log2(uFar + 1.0f);
    gl_FragDepth = log2(fsFlogZ) * Coef * 0.5f;

    // Smooth alpha fade toward the edges for anti-aliasing
    const float Dist = abs(fsEdgeDist);
    const float Edge = uThickness * 0.5f;
    const float Alpha = uOpacity * (1.0f - smoothstep(Edge - 1.0f, Edge, Dist));

    oFragColor = vec4(uColor, Alpha);
    oFragLocalPosition = vec4(0.0f);
    oFragWorldPosition = vec4(0.0f);
    oNodeInfo = vec4(float(uNodeId), 0.0f, 0.0f, 1.0f);
}
