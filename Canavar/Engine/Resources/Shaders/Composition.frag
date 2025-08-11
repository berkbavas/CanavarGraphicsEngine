#version 450 core

uniform sampler2D uSceneTexture;
uniform sampler2D uRaysTexture;

in vec2 fsTextureCoords;
out vec4 OutFragColor;

void main()
{
    vec3 scene = texture(uSceneTexture, fsTextureCoords).rgb;
    vec3 rays = texture(uRaysTexture, fsTextureCoords).rgb;
    OutFragColor = vec4(min(scene + rays, vec3(1.0f)), 1.0f);
}
