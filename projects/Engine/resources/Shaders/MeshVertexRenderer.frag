#version 430 core
layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

uniform int selectedVertexID = -1;
uniform vec4 vertexColor = vec4(1, 1, 1, 1);
uniform vec4 selectedVertexColor = vec4(0, 1, 0, 1);

flat in int fsVertexID;

void main()
{
    if (fsVertexID == selectedVertexID)
        fragColor = selectedVertexColor;
    else
        fragColor = vertexColor;

    brightColor = vec4(0);
}
