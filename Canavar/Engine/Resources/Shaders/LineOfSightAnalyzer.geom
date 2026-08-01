#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 uVPs[6];

in vec3 gsWorldPosition[];
out vec4 fsWorldPosition;

void main()
{
    // Render the triangle to each face of the cubemap.
    // 0: +X
    // 1: -X
    // 2: +Y
    // 3: -Y
    // 4: +Z
    // 5: -Z

    for (int Face = 0; Face < 6; ++Face)
    {
        gl_Layer = Face;                        // Built-in variable that specifies to which face we render.
        for (int Index = 0; Index < 3; ++Index) // For each triangle's vertices
        {
            fsWorldPosition = vec4(gsWorldPosition[Index], 1.0f);
            gl_Position = uVPs[Face] * fsWorldPosition;
            EmitVertex();
        }

        EndPrimitive();
    }
}
