#version 330 core
layout(location = 0) in vec3 point;
layout(location = 1) in float forkLevel;

uniform mat4 VP;

out float fsForkLevel;

void main()
{
    gl_Position = VP * vec4(point, 1);
	fsForkLevel = forkLevel;
}
