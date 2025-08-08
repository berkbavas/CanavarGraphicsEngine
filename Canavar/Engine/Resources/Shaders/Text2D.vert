#version 330 core

layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>

out vec2 fsTextureCoords;

uniform mat4 uProjection;

void main()
{
    gl_Position = uProjection * vec4(vertex.xy, 0.0, 1.0);
    fsTextureCoords = vertex.zw;
}