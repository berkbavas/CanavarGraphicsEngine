#version 330 core

in vec2 fsTextureCoords;
in vec3 fsLocalPosition;
in vec3 fsWorldPosition;
in float fsFlogZ;

uniform sampler2D uTextTexture;
uniform vec3 uTextColor;
uniform float uNodeId;
uniform float uZFar;

layout(location = 0) out vec4 OutFragColor;
layout(location = 1) out vec4 OutLocalPosition;
layout(location = 2) out vec4 OutWorldPosition;
layout(location = 3) out vec4 OutNodeInfo;

void main()
{
    OutFragColor = vec4(uTextColor, texture(uTextTexture, fsTextureCoords).r);
    OutLocalPosition = vec4(fsLocalPosition, 1.0f);
    OutWorldPosition = vec4(fsWorldPosition, 1.0f);
    OutNodeInfo = vec4(uNodeId, 0.0f, float(gl_PrimitiveID), 1.0f);

    gl_FragDepth = log2(fsFlogZ) / log2(uZFar + 1.0);
}