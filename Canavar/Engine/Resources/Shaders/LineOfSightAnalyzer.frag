#version 450 core

uniform vec3 uObserverPosition;
uniform float uFarPlane;

in vec4 fsWorldPosition;

void main()
{
    const vec3 FragmentToObserver = fsWorldPosition.xyz - uObserverPosition;
    const float Depth = length(FragmentToObserver) / uFarPlane;
    gl_FragDepth = Depth;   
}
