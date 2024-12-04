#version 430 core

uniform vec3 A, B, C, D, E, F, G, H, I, Z;
uniform vec3 sunDirection;
uniform float nodeId;

vec3 hosek_wilkie(float cos_theta, float gamma, float cos_gamma)
{
    vec3 chi = (1 + cos_gamma * cos_gamma) / pow(1 + H * H - 2 * cos_gamma * H, vec3(1.5));
    return (1 + A * exp(B / (cos_theta + 0.01))) * (C + D * exp(E * gamma) + F * (cos_gamma * cos_gamma) + G * chi + I * sqrt(cos_theta));
}

vec3 hosek_wilkie_sky_rgb(vec3 v, vec3 sun_direction)
{
    float cos_theta = clamp(v.y, 0, 1);
    float cos_gamma = clamp(dot(v, sun_direction), 0, 1);
    float gamma = acos(cos_gamma);

    vec3 R = Z * hosek_wilkie(cos_theta, gamma, cos_gamma);
    return R;
}

layout(location = 0) in vec3 fsDirection;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 brightColor;
layout(location = 4) out vec4 nodeInfo;

void main()
{
    vec3 color = hosek_wilkie_sky_rgb(fsDirection, sunDirection);

    fragColor = vec4(color, 1.0);
    brightColor = vec4(0);

    nodeInfo = vec4(nodeId, 0, 0, 1);
}
