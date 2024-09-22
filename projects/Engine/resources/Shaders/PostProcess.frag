#version 330 core
uniform sampler2D sceneTexture;
uniform sampler2D bloomBlurTexture;
uniform float exposure;
uniform float gamma;

in vec2 fsTextureCoords;

out vec4 outColor;

void main()
{
    vec3 hdrColor = texture(sceneTexture, fsTextureCoords).rgb;
    vec3 bloomColor = texture(bloomBlurTexture, fsTextureCoords).rgb;

    // Additive blending
    hdrColor += bloomColor;

    // Tone mapping
    // vec3 result = vec3(1) - exp(-hdrColor * exposure);

    // Also gamma correct while we're at it
    // result = pow(result, vec3(1.0f / gamma));

    outColor = vec4(hdrColor, 1);
}
