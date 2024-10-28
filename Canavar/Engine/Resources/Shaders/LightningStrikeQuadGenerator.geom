#version 450 core

layout(lines) in;
layout(triangle_strip, max_vertices = 32) out;

in vec3 gsWorldPosition[];
in float gsForkLevel[];

out vec4 fsColor;

uniform mat4 VP;
uniform vec3 viewDirection;
uniform float quadWidth;

void main()
{
    float forkLevel = gsForkLevel[1];
    vec3 p0 = gsWorldPosition[0];
    vec3 p1 = gsWorldPosition[1];
    vec3 dir = normalize(p1 - p0);
    vec3 perp = cross(dir, viewDirection);
    float myWidth = quadWidth / (forkLevel + 1);

    vec4 color = vec4(0.0f, 0.0f, 0.2f, 1.0f);

    gl_Position = VP * vec4(p0 + myWidth * perp, 1.0f);
    fsColor = color;
    EmitVertex();

    gl_Position = VP * vec4(p0, 1.0f);
    fsColor = vec4(1.0f);
    EmitVertex();

    gl_Position = VP * vec4(p1 + myWidth * perp, 1.0f);
    fsColor = color;
    EmitVertex();

    gl_Position = VP * vec4(p1, 1.0f);
    fsColor = vec4(1.0f);
    EmitVertex();
    EndPrimitive();

    gl_Position = VP * vec4(p0, 1.0f);
    fsColor = vec4(1.0f);
    EmitVertex();

    gl_Position = VP * vec4(p0 - myWidth * perp, 1.0f);
    fsColor = color;
    EmitVertex();

    gl_Position = VP * vec4(p1, 1.0f);
    fsColor = vec4(1.0f);
    EmitVertex();

    gl_Position = VP * vec4(p1 - myWidth * perp, 1.0f);
    fsColor = color;
    EmitVertex();
    EndPrimitive();
}