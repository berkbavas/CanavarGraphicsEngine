#version 430 core

uniform float maxRadius;
uniform float maxDistance;

layout(location = 0) in float fsRadius;
layout(location = 1) in float fsDistance;
layout(location = 2) in vec4 fsLocalPosition;
layout(location = 3) in vec4 fsWorldPosition;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 brightColor;
layout(location = 2) out vec4 fragLocalPosition;
layout(location = 3) out vec4 fragWorldPosition;

void main()
{
    vec4 result;

    if (fsRadius > 0.95 * maxRadius)
        result = vec4(128, 0, 0, 1);
    else if (fsRadius < 0.25)
        result = mix(vec4(1, 1, 1, 1), vec4(1, 0.75, 0, 1), pow(fsDistance / maxDistance, 4));
    else
        result = mix(vec4(1, 0.5, 0, 1), vec4(1, 1, 0, 1), fsRadius / maxRadius);

    fragColor = result;
    brightColor = result;
    fragLocalPosition = fsLocalPosition;
    fragWorldPosition = fsWorldPosition;
}
