#version 430 core

layout(location = 0) in vec2 aPosition;

uniform mat4 uIVP; // Inverse view-projection matrix
uniform float uSkyYOffset;

layout(location = 0) out vec3 fsDirection;

void main()
{
    vec4 clipPos = vec4(aPosition.xy + vec2(0, -uSkyYOffset), -1.0f, 1.0f);
    vec4 viewPos = uIVP * clipPos;
    fsDirection = normalize(viewPos.xyz);
    gl_Position = vec4(aPosition, -1.0f, 1.0f);
}
