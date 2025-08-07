// Taken from https://github.com/AntonHakansson/procedural-terrain
#version 450

// attributes of the input CPs
// define the number of CPs in the output patch
layout(vertices = 3) out;
// Outputs from VS are aggregated into arrays, based on the patch primitive.
in DATA
{
    vec3 world_pos;
    vec2 tex_coord;
    vec3 normal;
}
In[];

uniform vec3 eye_world_pos;
uniform float tess_multiplier;

// attributes of the output CPs
out DATA
{
    vec3 world_pos;
    vec2 tex_coord;
    vec3 normal;
}
Out[];

float getTessLevel(float distance0, float distance1)
{
    float avgDistance = (distance0 + distance1) / 2.0;
    // float maxDistance = 600.0;

    float res = 0;
    if (avgDistance > 3000.0)
    {
        res = 1;
    }
    else if (avgDistance > 2000.0)
    {
        res = 2;
    }
    else if (avgDistance > 1200.0)
    {
        res = 4;
    }
    else
    {
        res = 12;
    }

    return 1 + res * tess_multiplier;
}

void main()
{
    // Set the control points of the output patch
    Out[gl_InvocationID].world_pos = In[gl_InvocationID].world_pos;
    Out[gl_InvocationID].tex_coord = In[gl_InvocationID].tex_coord;
    Out[gl_InvocationID].normal = In[gl_InvocationID].normal;

    // Calculate the distance from the camera to the three control points
    float eyeToVertexDistance0 = distance(eye_world_pos, In[0].world_pos);
    float eyeToVertexDistance1 = distance(eye_world_pos, In[1].world_pos);
    float eyeToVertexDistance2 = distance(eye_world_pos, In[2].world_pos);

    // Calculate the tessellation levels
    gl_TessLevelOuter[0] = getTessLevel(eyeToVertexDistance1, eyeToVertexDistance2);
    gl_TessLevelOuter[1] = getTessLevel(eyeToVertexDistance2, eyeToVertexDistance0);
    gl_TessLevelOuter[2] = getTessLevel(eyeToVertexDistance0, eyeToVertexDistance1);
    gl_TessLevelInner[0] = gl_TessLevelOuter[2];
}
