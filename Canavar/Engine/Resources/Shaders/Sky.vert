#version 330 core

layout(location = 0) in vec2 aPos; // Use verts: (-1,-1), (3,-1), (-1,3)

uniform mat4 uInvProjectionMatrix;
uniform mat4 uInvViewMatrix;
uniform float uHorizonOffset;

out vec3 fsViewDir;

void main()
{
    gl_Position = vec4(aPos, 0.0, 1.0);

    // clip -> eye -> world, build world-space view ray direction
    vec4 Clip = vec4(aPos, 0.0, 1.0);
    vec4 Eye = uInvProjectionMatrix * Clip;
    Eye.z = -1.0; // forward
    Eye.w = 0.0;  // vector
    vec4 World = uInvViewMatrix * Eye;
    World.y += uHorizonOffset;

    fsViewDir = normalize(World.xyz);
}