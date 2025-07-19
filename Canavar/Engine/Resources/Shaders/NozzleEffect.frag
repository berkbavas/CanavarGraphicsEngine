#version 430 core

uniform float maxRadius;

in vec3 fsPosition;

layout(location = 0) out vec4 fragColor;

void main()
{
    float x = fsPosition.x;
    float y = fsPosition.y;
    float r = sqrt(x * x + y * y);
    float nr = r / maxRadius;

    if (r < 0.1)
    {
        fragColor = mix(vec4(1), vec4(1, 1, 0, 1), nr);
    }
    else
    {
        fragColor = mix(vec4(1, 1, 0, 1), vec4(1, 0, 0, 1), (nr - 0.5) / 0.5);
    }
}
