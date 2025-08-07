#version 450

// Taken from https://github.com/AntonHakansson/procedural-terrain

layout(location = 0) in vec3 pos_in;
layout(location = 1) in vec2 offset_in;

uniform float width;

// Out data
out DATA
{
    vec3 world_pos;
    vec2 tex_coord;
    vec3 normal;
}
Out;

void main()
{
    // NOTE: We transform the point into world space and not clip space for the tesselation control shader
    Out.world_pos = pos_in;
    Out.tex_coord = Out.world_pos.xz / width;
    Out.world_pos.xz += offset_in;
}
