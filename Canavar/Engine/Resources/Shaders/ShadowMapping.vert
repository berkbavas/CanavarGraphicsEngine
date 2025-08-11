#version 450 core

layout(location = 0) in vec4 aPosition;

uniform mat4 uLightViewProjectionMatrix; // Light View Projection Matrix
uniform mat4 uModelMatrix;   // Model Matrix

void main()
{
    gl_Position = uLightViewProjectionMatrix * uModelMatrix * vec4(aPosition.xyz, 1.0);
}
