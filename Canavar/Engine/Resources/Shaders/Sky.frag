#version 450 core

uniform vec3 A, B, C, D, E, F, G, H, I, Z;
uniform vec3 uSunDirection;
uniform float uNodeId;

layout(location = 0) in vec3 fsDirection;

layout(location = 0) out vec4 OutFragColor;
layout(location = 3) out vec4 OutNodeInfo;

vec3 HosekWilkie(float cos_theta, float gamma, float cos_gamma)
{
    vec3 chi = (1 + cos_gamma * cos_gamma) / pow(1 + H * H - 2 * cos_gamma * H, vec3(1.5));
    return (1 + A * exp(B / (cos_theta + 0.01))) * (C + D * exp(E * gamma) + F * (cos_gamma * cos_gamma) + G * chi + I * sqrt(cos_theta));
}

vec3 HosekWilkieSkyRgb(vec3 v, vec3 sun_direction)
{
    float cos_theta = clamp(v.y, 0, 1);
    float cos_gamma = clamp(dot(v, sun_direction), 0, 1);
    float gamma = acos(cos_gamma);

    vec3 R = Z * HosekWilkie(cos_theta, gamma, cos_gamma);
    return R;
}

void main()
{
    OutFragColor = vec4(HosekWilkieSkyRgb(fsDirection, uSunDirection), 1.0f);
    OutNodeInfo = vec4(float(uNodeId), 0.0f, 0.0f, 1.0f);
}
