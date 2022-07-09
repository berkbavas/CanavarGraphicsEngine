#version 330 core
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;

out vec3 fPosition;
out vec3 fNormal;

struct Node {
    mat4 transformation;
    vec4 color;
    float ambient;
    float diffuse;
    float specular;
    float shininess;
};

uniform Node node;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    fPosition = vec3(node.transformation * vec4(vPosition, 1.0));
    fNormal = mat3(transpose(inverse(node.transformation))) * vNormal;

    gl_Position = projectionMatrix * viewMatrix * vec4(fPosition, 1.0);
}
