#version 430 core

in float fsFlogZ;

uniform vec3 uColor;
uniform float uFar;

layout(location = 0) out vec4 oFragColor;
layout(location = 1) out vec4 oFragLocalPosition;
layout(location = 2) out vec4 oFragWorldPosition;
layout(location = 3) out vec4 oNodeInfo;

void main()
{
    const float Coef = 2.0f / log2(uFar + 1.0f);
    gl_FragDepth = log2(fsFlogZ) * Coef * 0.5f;

    oFragColor = vec4(uColor, 1.0f);
    oFragLocalPosition = vec4(0.0f);
    oFragWorldPosition = vec4(0.0f);
    oNodeInfo = vec4(0.0f);
}
