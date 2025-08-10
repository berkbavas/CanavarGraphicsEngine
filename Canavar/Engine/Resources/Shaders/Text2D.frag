#version 330 core

in vec2 fsTextureCoords;
in vec3 fsLocalPosition;
in vec3 fsWorldPosition;

uniform sampler2D uTextTexture;
uniform vec3 uTextColor;
uniform float uNodeId;

layout(location = 0) out vec4 OutFragColor;
layout(location = 1) out vec4 OutLocalPosition;
layout(location = 2) out vec4 OutWorldPosition;
layout(location = 3) out vec4 OutNodeInfo;
layout(location = 4) out vec4 OutFragVelocity;

void main()
{
    OutFragColor = vec4(uTextColor, texture(uTextTexture, fsTextureCoords).r);
    OutLocalPosition = vec4(fsLocalPosition, 1.0f);
    OutWorldPosition = vec4(fsWorldPosition, 1.0f);
    OutNodeInfo = vec4(uNodeId, 0.0f, float(gl_PrimitiveID), 1.0f);
    OutFragVelocity = vec4(0.0f, 0.0f, 0.0f, 1.0f); // Placeholder for velocity output
}