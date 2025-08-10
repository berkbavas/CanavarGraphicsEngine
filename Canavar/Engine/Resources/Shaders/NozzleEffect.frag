#version 430 core

uniform float uMaxRadius;
uniform float uZFar;

in vec3 fsPosition;
in float fsFlogZ;

layout(location = 0) out vec4 OutFragColor;
layout(location = 4) out vec4 OutFragVelocity;

void main()
{
    float x = fsPosition.x;
    float y = fsPosition.y;
    float r = sqrt(x * x + y * y);
    float nr = r / uMaxRadius;

    if (r < 0.1)
    {
        OutFragColor = mix(vec4(1), vec4(1, 1, 0, 1), nr);
    }
    else
    {
        OutFragColor = mix(vec4(1, 1, 0, 1), vec4(1, 0, 0, 1), (nr - 0.5) / 0.5);
    }

    OutFragVelocity = vec4(0.0f, 0.0f, 0.0f, 1.0f); // Placeholder for velocity output
    gl_FragDepth = log2(fsFlogZ) / log2(uZFar + 1.0);
}
