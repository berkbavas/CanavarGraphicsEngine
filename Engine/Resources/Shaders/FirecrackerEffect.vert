#version 330 core
layout(location = 0) in vec3 cubeVertexPosition;
layout(location = 1) in vec3 position;
layout(location = 2) in float life;
layout(location = 3) in float deadAfter;

uniform mat4 MVP;
uniform float scale;
uniform float maxLife;

out vec3 fsPosition;
out float fsLife;
out float fsDeadAfter;

void main()
{
    fsPosition = position;
    fsLife = life;
    fsDeadAfter = deadAfter;

    gl_Position = MVP * vec4(position + scale * cubeVertexPosition, life >= deadAfter ? 0.0f : 1.0f);
}
