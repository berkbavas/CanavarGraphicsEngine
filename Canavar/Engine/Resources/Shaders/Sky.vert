#version 430 core

layout(location = 0) in vec2 position;

uniform mat4 IVP; // Inverse view-projection matrix
uniform float skyYOffset;

layout(location = 0) out vec3 fsDirection;

void main()
{
    vec4 clipPos = vec4(position.xy + vec2(0, -skyYOffset), -1.0f, 1.0f);
    vec4 viewPos = IVP * clipPos;
    fsDirection = normalize(viewPos.xyz);
    gl_Position = vec4(position, -1.0f, 1.0f);
}
