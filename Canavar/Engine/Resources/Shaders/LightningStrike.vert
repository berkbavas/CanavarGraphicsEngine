#version 430 core

layout(location = 0) in vec3 aWorldPosition;
layout(location = 1) in float aForkLevel;

out vec3 gsWorldPosition;
out float gsForkLevel;

void main()
{
    gsWorldPosition = aWorldPosition;
    gsForkLevel = aForkLevel;
}
