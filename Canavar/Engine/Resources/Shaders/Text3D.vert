#version 330 core

layout(location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>

out vec2 fsTextureCoords;
out vec3 fsLocalPosition;
out vec3 fsWorldPosition;

uniform mat4 uMVP;
uniform mat4 uModelMatrix;

void main()
{
    fsLocalPosition = vec3(vertex.xy, 0.0);
    fsWorldPosition = (uModelMatrix * vec4(fsLocalPosition, 1.0)).xyz;
    gl_Position = uMVP * vec4(fsLocalPosition, 1.0);
    fsTextureCoords = vertex.zw;
}