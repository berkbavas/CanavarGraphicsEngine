#version 450 core

layout(triangles) in;

struct Plane
{
    vec3 Point;
    vec3 Normal;
};

uniform Plane uPlane;

uniform mat4 uVP;

layout(line_strip, max_vertices = 3) out;

// Reference: https://stackoverflow.com/questions/3142469/determining-the-intersection-of-a-triangle-and-a-plane

// Assume the plane is given as the equation dot(N,X) + d = 0, where N is a (not
// neccessarily normalized) plane normal, and d is a scalar. Any way the plane is given -
// DistFromPlane should just let the input vector into the plane equation.

const float PlaneD = -dot(uPlane.Normal, uPlane.Point);

float DistFromPlane(vec3 P)
{
    return dot(uPlane.Normal, P) + PlaneD;
}

bool GetSegmentPlaneIntersection(vec3 P1, vec3 P2, out vec3 OutP)
{
    float d1 = DistFromPlane(P1);
    float d2 = DistFromPlane(P2);

    if (d1 * d2 > 0) // points on the same side of plane
        return false;

    float t = d1 / (d1 - d2); // 'time' of intersection point on the segment
    OutP = P1 + t * (P2 - P1);

    return true;
}

void main()
{
    vec3 TriangleA = gl_in[0].gl_Position.xyz;
    vec3 TriangleB = gl_in[1].gl_Position.xyz;
    vec3 TriangleC = gl_in[2].gl_Position.xyz;

    vec3 IntersectionPoint;

    if (GetSegmentPlaneIntersection(TriangleA, TriangleB, IntersectionPoint))
    {
        gl_Position = uVP * vec4(IntersectionPoint, 1.0f);
        EmitVertex();
    }

    if (GetSegmentPlaneIntersection(TriangleB, TriangleC, IntersectionPoint))
    {
        gl_Position = uVP * vec4(IntersectionPoint, 1.0f);
        EmitVertex();
    }

    if (GetSegmentPlaneIntersection(TriangleC, TriangleA, IntersectionPoint))
    {
        gl_Position = uVP * vec4(IntersectionPoint, 1.0f);
        EmitVertex();
    }
}