#version 430 core

layout(location = 0) in vec2 position; // screen quad
layout(location = 1) in vec2 attribUV;

out vec3 rayDirWorldspace; // direction of vertex on focal plane f=1 in world space
out vec2 uv;

//uniform mat4 projView;

uniform float xMax, yMax; // rayDirWorldspace lies within [-xMax, xMax] x [-yMax, yMax] x {1.0}
uniform mat4 viewInverse;

void main()
{
    vec4 rayDirCameraspace = vec4(position.x * xMax, position.y * yMax, -1.f, 0.f);
    rayDirWorldspace = vec3(viewInverse * rayDirCameraspace);

    uv = attribUV;
    gl_Position = vec4(position, 0.0f, 1.0f); // quad position

    // DEBUG
    //    rayDirWorldspace = vec3(position);
    //    gl_Position = vec4(0.f, 0.f, 0.f, 1.f);
    //    gl_Position = vec4(positionObj, 1.f);
}
