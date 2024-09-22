#version 330 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 initialPosition;
layout(location = 2) in vec3 direction;
layout(location = 3) in float life;

uniform mat4 MVP;
uniform float scale;
uniform float speed;

out float fsRadius;
out float fsDistance;

void main()
{
    vec3 position = speed * direction * life + initialPosition +  scale * vertexPosition;
    gl_Position = MVP * vec4(position, 1.0f);
    fsRadius = length(initialPosition);
    fsDistance = position.z;
}
