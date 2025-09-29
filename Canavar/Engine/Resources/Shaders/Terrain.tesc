#version 450 core

// Taken from https://github.com/AntonHakansson/procedural-terrain

layout(vertices = 3) out;

// Outputs from VS are aggregated into arrays, based on the patch primitive.
in vec3 tcWorldPosition[];
in vec2 tcTextureCoords[];
in vec3 tcNormal[];

uniform vec3 uCameraPosition;
uniform float uTessellationMultiplier;

// Attributes of the output CPs
out vec3 teWorldPosition[];
out vec2 teTextureCoords[];
out vec3 teNormal[];

float GetTessLevel(float Distance0, float Distance1)
{
    float AvgDistance = (Distance0 + Distance1) / 2.0f;

    float Result = 0;
    if (AvgDistance > 3000.0f)
    {
        Result = 1;
    }
    else if (AvgDistance > 2000.0f)
    {
        Result = 2;
    }
    else if (AvgDistance > 1200.0f)
    {
        Result = 4;
    }
    else
    {
        Result = 12;
    }

    return 1 + Result * uTessellationMultiplier;
}

void main()
{
    // Set the control points of the output patch
    teWorldPosition[gl_InvocationID] = tcWorldPosition[gl_InvocationID];
    teTextureCoords[gl_InvocationID] = tcTextureCoords[gl_InvocationID];
    teNormal[gl_InvocationID] = tcNormal[gl_InvocationID];

    // Calculate the distance from the camera to the three control points
    float EyeToVertexDistance0 = distance(uCameraPosition, tcWorldPosition[0]);
    float EyeToVertexDistance1 = distance(uCameraPosition, tcWorldPosition[1]);
    float EyeToVertexDistance2 = distance(uCameraPosition, tcWorldPosition[2]);

    // Calculate the tessellation levels
    gl_TessLevelOuter[0] = GetTessLevel(EyeToVertexDistance1, EyeToVertexDistance2);
    gl_TessLevelOuter[1] = GetTessLevel(EyeToVertexDistance2, EyeToVertexDistance0);
    gl_TessLevelOuter[2] = GetTessLevel(EyeToVertexDistance0, EyeToVertexDistance1);
    gl_TessLevelInner[0] = gl_TessLevelOuter[2];
}
