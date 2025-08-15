#version 450 core

in vec2 fsTextureCoords;

uniform sampler2D uTextTexture;
uniform vec3 uTextColor;
uniform float uNodeId;

layout(location = 0) out vec4 OutFragColor;
layout(location = 3) out vec4 OutNodeInfo;

void main()
{
    OutFragColor = vec4(uTextColor, texture(uTextTexture, fsTextureCoords).r);
    OutNodeInfo = vec4(uNodeId, 0.0f, float(gl_PrimitiveID), 1.0f);
}