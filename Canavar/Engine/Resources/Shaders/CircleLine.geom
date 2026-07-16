#version 430 core

// Each line segment is expanded into a screen-space quad of `uThickness` pixels.
layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

in float gsFlogZ[];

out float fsFlogZ;
out float fsEdgeDist; // signed distance from the centre line in pixels

uniform vec2 uViewportSize; // (width, height) in pixels
uniform float uThickness;   // line thickness in pixels

void main()
{
    const vec4 P0 = gl_in[0].gl_Position;
    const vec4 P1 = gl_in[1].gl_Position;

    // Perspective-divide to NDC, then to screen space
    const vec2 Screen0 = (P0.xy / P0.w * 0.5f + 0.5f) * uViewportSize;
    const vec2 Screen1 = (P1.xy / P1.w * 0.5f + 0.5f) * uViewportSize;

    const vec2 Dir = Screen1 - Screen0;
    const vec2 Normal = normalize(vec2(-Dir.y, Dir.x)); // perpendicular in screen space

    const float HalfT = uThickness * 0.5f;

    // Convert the screen-space offset back to clip space (account for w)
    const vec2 Offset0 = Normal * HalfT / uViewportSize * 2.0f * P0.w;
    const vec2 Offset1 = Normal * HalfT / uViewportSize * 2.0f * P1.w;

    // Emit 4 vertices of the quad (triangle strip)
    fsFlogZ = gsFlogZ[0];
    fsEdgeDist = -HalfT;
    gl_Position = vec4(P0.xy + Offset0, P0.zw);
    EmitVertex();

    fsFlogZ = gsFlogZ[0];
    fsEdgeDist = HalfT;
    gl_Position = vec4(P0.xy - Offset0, P0.zw);
    EmitVertex();

    fsFlogZ = gsFlogZ[1];
    fsEdgeDist = -HalfT;
    gl_Position = vec4(P1.xy + Offset1, P1.zw);
    EmitVertex();

    fsFlogZ = gsFlogZ[1];
    fsEdgeDist = HalfT;
    gl_Position = vec4(P1.xy - Offset1, P1.zw);
    EmitVertex();

    EndPrimitive();
}
