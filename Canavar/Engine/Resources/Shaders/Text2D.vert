#version 450 core

layout (location = 0) in vec4 aVertex; // <vec2 pos, vec2 tex>

out vec2 fsTextureCoords;

uniform mat4 uProjection;

void main()
{
    gl_Position = uProjection * vec4(aVertex.xy, 0.0, 1.0);
    fsTextureCoords = aVertex.zw;
}