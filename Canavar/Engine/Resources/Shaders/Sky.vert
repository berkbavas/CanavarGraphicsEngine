#version 330 core
layout(location = 0) in vec2 aPos; // Use verts: (-1,-1), (3,-1), (-1,3)


uniform mat4 uInvProjectionMatrix; // inverse projection
uniform mat4 uInvViewMatrix;       // inverse view
uniform float uHorizonOffset;


out vec3 vViewDir;

void main()
{
    gl_Position = vec4(aPos , 0.0, 1.0);

    // clip -> eye -> world, build world-space view ray direction
    vec4 clip = vec4(aPos, 0.0, 1.0);
    clip.y += uHorizonOffset;
    vec4 eye = uInvProjectionMatrix * clip;
    eye.z = -1.0; // forward
    eye.w = 0.0;  // vector
    vec4 world = uInvViewMatrix * eye;

    vViewDir = normalize(world.xyz);
}