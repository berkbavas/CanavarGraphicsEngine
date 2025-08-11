#version 450 core

// Taken from https://github.com/AntonHakansson/procedural-terrain

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aOffset;

uniform float uWidth;

out vec3 tcWorldPosition;
out vec2 tcTextureCoords;
out vec3 tcNormal;

void main()
{
    // NOTE: We transform the point into world space and not clip space for the tesselation control shader
    tcWorldPosition = aPosition;
    tcTextureCoords = tcWorldPosition.xz / uWidth;
    tcWorldPosition.xz += aOffset;
    tcNormal = vec3(0.0f, 1.0f, 0.0f); // Placeholder for normal
}
