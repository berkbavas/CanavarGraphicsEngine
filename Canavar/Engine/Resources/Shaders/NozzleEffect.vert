#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 initialPosition;
layout(location = 2) in vec3 direction;
layout(location = 3) in float life;

uniform mat4 M;
uniform mat4 VP;
uniform float scale;
uniform float speed;

layout(location = 0) out float fsRadius;
layout(location = 1) out float fsDistance;
layout(location = 2) out vec4 fsLocalPosition;
layout(location = 3) out vec4 fsWorldPosition;

void main()
{
    fsLocalPosition = vec4(speed * direction * life + initialPosition + scale * vertexPosition, 1.0f);
    fsWorldPosition = M * fsLocalPosition;
    fsRadius = length(initialPosition);
    fsDistance = fsLocalPosition.z;

    gl_Position = VP * fsWorldPosition;
}
