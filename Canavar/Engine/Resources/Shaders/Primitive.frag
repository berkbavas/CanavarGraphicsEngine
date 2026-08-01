#version 430 core



uniform vec3 uColor;
uniform float uOpacity;
uniform int uNodeId;
uniform float uFar;
uniform vec3 uCameraPosition;

in vec3 fsFragWorldPosition;
in vec3 fsFragLocalPosition;
in vec3 fsNormal;
in float fsFlogZ;

layout(location = 0) out vec4 oFragColor;
layout(location = 1) out vec4 oFragLocalPosition;
layout(location = 2) out vec4 oFragWorldPosition;
layout(location = 3) out vec4 oNodeInfo;

void main()
{
    // Logarithmic depth
    const float Coef = 2.0f / log2(uFar + 1.0f);
    gl_FragDepth = log2(fsFlogZ) * Coef * 0.5f;

    // Simple diffuse + ambient from the camera direction
    const vec3 ViewDir = normalize(uCameraPosition - fsFragWorldPosition);
    const float Diffuse = max(dot(normalize(fsNormal), ViewDir), 0.0f);
    const vec3 ShadedColor = uColor * (0.3f + 0.7f * Diffuse);

    oFragColor = vec4(ShadedColor, uOpacity);
    oFragLocalPosition = vec4(0.0f);
    oFragWorldPosition = vec4(fsFragWorldPosition, 1.0f);
    oNodeInfo = vec4(float(uNodeId), 0.0f, 0.0f, 1.0f);
}
