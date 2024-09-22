#version 430 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 textureCoord;
layout(location = 3) in vec2 offsetPosition;

uniform mat4 M;

out vec3 tcsPosition;
out vec3 tcsNormal;
out vec2 tcsTextureCoord;

void main()
{
    tcsPosition = vec3(M * vec4(position, 1.0));
    tcsPosition.xz += offsetPosition;
    tcsNormal = normal;
    tcsTextureCoord = textureCoord;
}
