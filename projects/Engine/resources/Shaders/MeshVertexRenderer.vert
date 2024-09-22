#version 330 core
layout(location = 0) in vec3 cubeVertexPosition;
layout(location = 1) in vec3 meshVertexPosition;
layout(location = 2) in vec3 meshVertexNormal;

uniform float scale = 0.025f;
uniform mat4 MVP;

flat out int fsVertexID;

void main()
{
    gl_Position = MVP * vec4(meshVertexPosition + scale * cubeVertexPosition, 1);
    fsVertexID = gl_InstanceID;
}
