#version 450 core

uniform sampler2D uSceneTexture;
uniform float uThreshold;

in vec2 fsTextureCoords;

out vec4 OutFragColor;

void main()
{
    vec3 color = texture(uSceneTexture, fsTextureCoords).rgb;
    float brightness = dot(color, vec3(0.2126f, 0.7152f, 0.0722f));
    OutFragColor = brightness > uThreshold ? vec4(color, 1.0f) : vec4(0.0f);
}