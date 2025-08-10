#version 450

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

float GetTessLevel(float distance0, float distance1)
{
    float avgDistance = (distance0 + distance1) / 2.0f;

    float res = 0;
    if (avgDistance > 3000.0f)
    {
        res = 1;
    }
    else if (avgDistance > 2000.0f)
    {
        res = 2;
    }
    else if (avgDistance > 1200.0f)
    {
        res = 4;
    }
    else
    {
        res = 12;
    }

    return 1 + res * uTessellationMultiplier;
}

void main()
{
    // Set the control points of the output patch
    teWorldPosition[gl_InvocationID] = tcWorldPosition[gl_InvocationID];
    teTextureCoords[gl_InvocationID] = tcTextureCoords[gl_InvocationID];
    teNormal[gl_InvocationID] = tcNormal[gl_InvocationID];

    // Calculate the distance from the camera to the three control points
    float eyeToVertexDistance0 = distance(uCameraPosition, tcWorldPosition[0]);
    float eyeToVertexDistance1 = distance(uCameraPosition, tcWorldPosition[1]);
    float eyeToVertexDistance2 = distance(uCameraPosition, tcWorldPosition[2]);

    // Calculate the tessellation levels
    gl_TessLevelOuter[0] = GetTessLevel(eyeToVertexDistance1, eyeToVertexDistance2);
    gl_TessLevelOuter[1] = GetTessLevel(eyeToVertexDistance2, eyeToVertexDistance0);
    gl_TessLevelOuter[2] = GetTessLevel(eyeToVertexDistance0, eyeToVertexDistance1);
    gl_TessLevelInner[0] = gl_TessLevelOuter[2];
}
