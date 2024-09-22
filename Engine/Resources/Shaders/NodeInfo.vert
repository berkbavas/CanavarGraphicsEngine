#version 330 core
layout(location = 0) in vec3 position;

uniform mat4 MVP;

flat out int fsVertexID; // Do not interpolate

void main()
{
    fsVertexID = gl_VertexID;
    gl_Position = MVP * vec4(position, 1);
}
