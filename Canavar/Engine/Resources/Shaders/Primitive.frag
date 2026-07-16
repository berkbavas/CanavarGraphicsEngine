#version 430 core

in vec3 fsNormal;
in vec3 fsFragWorldPosition;
in float fsFlogZ;

uniform vec3 uColor;
uniform float uOpacity;
uniform int uNodeId;
uniform float uFar;
uniform vec3 uCameraPosition;

layout(location = 0) out vec4 OutFragColor;
layout(location = 1) out vec4 OutFragLocalPosition;
layout(location = 2) out vec4 OutFragWorldPosition;
layout(location = 3) out vec4 OutNodeInfo;

void main()
{
    // Logarithmic depth
    const float Coef = 2.0f / log2(uFar + 1.0f);
    gl_FragDepth = log2(fsFlogZ) * Coef * 0.5f;

    // Simple diffuse + ambient from the camera direction
    const vec3 ViewDir = normalize(uCameraPosition - fsFragWorldPosition);
    const float Diffuse = max(dot(normalize(fsNormal), ViewDir), 0.0f);
    const vec3 ShadedColor = uColor * (0.3f + 0.7f * Diffuse);

    OutFragColor = vec4(ShadedColor, uOpacity);
    OutFragLocalPosition = vec4(0.0f);
    OutFragWorldPosition = vec4(fsFragWorldPosition, 1.0f);
    OutNodeInfo = vec4(float(uNodeId), 0.0f, 0.0f, 1.0f);
}
