#version 330 core
layout(location = 0) in vec3 worldPosition;
layout(location = 1) in float forkLevel;

out vec3 gsWorldPosition;
out float gsForkLevel;

void main()
{
    gsWorldPosition = worldPosition;
	gsForkLevel = forkLevel;
}
