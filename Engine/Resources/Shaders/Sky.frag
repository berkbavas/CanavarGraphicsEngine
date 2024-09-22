#version 330 core

uniform vec3 A, B, C, D, E, F, G, H, I, Z;
uniform vec3 sunDir;

vec3 hosek_wilkie(float cos_theta, float gamma, float cos_gamma)
{
    vec3 chi = (1 + cos_gamma * cos_gamma) / pow(1 + H * H - 2 * cos_gamma * H, vec3(1.5));
    return (1 + A * exp(B / (cos_theta + 0.01))) * (C + D * exp(E * gamma) + F * (cos_gamma * cos_gamma) + G * chi + I * sqrt(cos_theta));
}

vec3 hosek_wilkie_sky_rgb(vec3 v, vec3 sun_dir)
{
    float cos_theta = clamp(v.y, 0, 1);
    float cos_gamma = clamp(dot(v, sun_dir), 0, 1);
    float gamma = acos(cos_gamma);

    vec3 R = Z * hosek_wilkie(cos_theta, gamma, cos_gamma);
    return R;
}

in vec3 fsDirection;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

void main()
{
    vec3 color = hosek_wilkie_sky_rgb(fsDirection, sunDir);

    fragColor = vec4(color, 1.0);
    brightColor = vec4(0);
}
