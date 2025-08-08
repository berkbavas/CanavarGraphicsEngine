#version 330 core

layout(location = 0) in vec4 aVertex; // <vec2 pos, vec2 tex>

out vec2 fsTextureCoords;
out vec3 fsLocalPosition;
out vec3 fsWorldPosition;
out float fsFlogZ;

uniform float uZFar; 
uniform mat4 uMVP;
uniform mat4 uModelMatrix;

void main()
{
    fsLocalPosition = vec3(aVertex.xy, 0.0);
    fsWorldPosition = (uModelMatrix * vec4(fsLocalPosition, 1.0)).xyz;
    gl_Position = uMVP * vec4(fsLocalPosition, 1.0);
    fsTextureCoords = aVertex.zw;

    float coef = 2.0 / log2(uZFar + 1.0);
    gl_Position.z = log2(max(1e-6, 1.0 + gl_Position.w)) * coef - 1.0;
    fsFlogZ = 1.0 + gl_Position.w;
}