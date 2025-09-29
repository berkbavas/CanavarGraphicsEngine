#version 450 core

uniform float uMaxRadius;
uniform float uFarPlane;

in vec3 fsPosition;
in float fsFlogZ;

layout(location = 0) out vec4 OutFragColor;

void main()
{
    // Nozzle effect based on distance from the center
    // Center -> Yellow, Edge -> Orange -> Red
    float distanceFromCenter = length(fsPosition.xy);
    float t = clamp(distanceFromCenter / uMaxRadius, 0.0, 1.0);
    t = smoothstep(0.0, 1.0, t);
    vec3 Color = mix(vec3(1.0, 1.0, 0.0), vec3(1.0, 0.5, 0.0), t);
    Color = mix(Color, vec3(1.0, 0.0, 0.0), t * t);
    OutFragColor = vec4(Color, 1.0);
    gl_FragDepth = log2(fsFlogZ) / log2(uFarPlane + 1.0);
}
