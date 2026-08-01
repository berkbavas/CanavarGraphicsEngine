#version 450 core

uniform vec3 uObserverPosition;
uniform float uFarPlane;

in vec4 fsWorldPosition;

void main()
{
    const float Depth = length(uObserverPosition - fsWorldPosition.xyz) / uFarPlane;
    gl_FragDepth = Depth;
}
