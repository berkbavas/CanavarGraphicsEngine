#version 430 core

in float fsFlogZ;

uniform vec3 uColor;
uniform float uFar;

layout(location = 0) out vec4 OutFragColor;
layout(location = 1) out vec4 OutFragLocalPosition;
layout(location = 2) out vec4 OutFragWorldPosition;
layout(location = 3) out vec4 OutNodeInfo;

void main()
{
    const float Coef = 2.0f / log2(uFar + 1.0f);
    gl_FragDepth = log2(fsFlogZ) * Coef * 0.5f;

    OutFragColor = vec4(uColor, 1.0f);
    OutFragLocalPosition = vec4(0.0f);
    OutFragWorldPosition = vec4(0.0f);
    OutNodeInfo = vec4(0.0f);
}
