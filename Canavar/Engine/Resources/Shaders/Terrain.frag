#version 450 core

// Taken from https://github.com/AntonHakansson/procedural-terrain

#define PI 3.14159265359

struct Terrain
{
    float Ambient;
    float Diffuse;
    float Specular;
    float Shininess;
};

struct Haze
{
    bool Enabled;
    vec3 Color;
    float Density;
    float Gradient;
};

struct Noise
{
    int Octaves;
    float Amplitude;
    float Frequency;
    float Persistence;
    float Lacunarity;
};

struct DirectionalLight
{
    vec3 Color;
    vec3 Direction;
    float Ambient;
    float Diffuse;
    float Specular;
    float Radiance;
};

struct PointLight
{
    vec3 Color;
    vec3 Position;
    float Ambient;
    float Diffuse;
    float Specular;
    float Constant;
    float Linear;
    float Quadratic;
};

layout(binding = 0) uniform sampler2DArray uAlbedos;
layout(binding = 1) uniform sampler2DArray uNormals;
layout(binding = 2) uniform sampler2DArray uDisplacement;

in vec3 fsWorldPosition;
in vec2 fsTextureCoords;
in vec3 fsNormal;
in vec3 fsTangent;
in vec3 fsBitangent;
in mat3 fsTangentMatrix;
in float fsLogZ;

layout(location = 0) out vec4 OutFragColor;
layout(location = 1) out vec4 OutFragLocalPosition;
layout(location = 2) out vec4 OutFragWorldPosition;
layout(location = 3) out vec4 OutFragNodeInfo;

uniform vec3 uCameraPosition;
uniform float uFarPlane;

uniform float uTextureStartHeights[4];
uniform float uTextureBlends[4];
uniform float uTextureSizes[4];
uniform float uTextureDisplacementWeights[4];

uniform Noise uNoise;
uniform Haze uHaze;
uniform Terrain uTerrain;

uniform PointLight uPointLights[8];
uniform int uNumPointLights;

uniform DirectionalLight uDirectionalLights[8]; // First element is the Sun
uniform int uNumDirectionalLights;

uniform int uNodeId;

/**
 * Functions
 */

float InverseLerp(float a, float b, float x)
{
    return (x - a) / (b - a);
}

float InverseLerpClamped(float a, float b, float x)
{
    return clamp(InverseLerp(a, b, x), 0, 1);
}

vec3 GetWorldNormal(vec3 textureCoords)
{
    vec3 TangentNormal = normalize(2.0 * texture(uNormals, textureCoords).xyz - 1.0f);
    return fsTangentMatrix * TangentNormal;
}

float[4] GetTextureWeightsByDisplacement(vec3[4] t, float[4] a)
{
    const float Depth = 0.1f;

    float Disp[4];
    for (int i = 0; i < 4; i++)
    {
        Disp[i] = texture(uDisplacement, t[i]).r * uTextureDisplacementWeights[i];
    }

    float Ma = 0;
    for (int i = 0; i < 4; i++)
    {
        Ma = max(Ma, Disp[i] + a[i]);
    }
    Ma -= Depth;

    float TotalWeight = 0;
    float[4] Weights;
    for (int i = 0; i < 4; i++)
    {
        Weights[i] = max(Disp[i] + a[i] - Ma, 0);
        TotalWeight += Weights[i];
    }

    // normalize
    for (int i = 0; i < 4; i++)
    {
        Weights[i] /= TotalWeight;
    }

    return Weights;
}

float[4] TerrainBlending(vec3 WorldPos, vec3 Normal)
{
    float Height = WorldPos.y;

    // A completely flat terrain has slope=0
    float Slope = max(1 - dot(Normal, vec3(0, 1, 0)), 0.0f);

    // For each fragment we compute how much each texture contributes depending on height and slope
    float DrawStrengths[4];

    float SandGrassHeight = uTextureStartHeights[0];
    float GrassRockHeight = uTextureStartHeights[1];
    float RockSnowHeight = uTextureStartHeights[2];

    float GrassFalloff = uTextureBlends[1];
    float RockFalloff = uTextureBlends[2];
    float SnowFalloff = uTextureBlends[3];

    float a, b, c, d;
    {
        b = InverseLerpClamped(SandGrassHeight - GrassFalloff / 2, SandGrassHeight + GrassFalloff / 2, Height);
    }
    {
        c = InverseLerpClamped(GrassRockHeight - RockFalloff / 2, GrassRockHeight + RockFalloff / 2, Height);
    }

    float b_in, c_in, d_in;
    b_in = InverseLerpClamped(SandGrassHeight - GrassFalloff / 2, SandGrassHeight + GrassFalloff / 2, Height);
    c_in = InverseLerpClamped(GrassRockHeight - RockFalloff / 2, GrassRockHeight + RockFalloff / 2, Height);
    d_in = InverseLerpClamped(RockSnowHeight - SnowFalloff / 2, RockSnowHeight + SnowFalloff / 2, Height);

    a = 1 - b_in;
    b *= 1 - c_in;
    c *= 1 - d_in;
    d = d_in;

    b *= 1 - InverseLerpClamped(0.1f, 1.0f, Slope);
    d *= 1 - InverseLerpClamped(0.1f, 1.0f, Slope);

    float Total = a + b + c + d;
    DrawStrengths[0] = max(a / Total, 0);
    DrawStrengths[1] = max(b / Total, 0);
    DrawStrengths[2] = max(c / Total, 0);
    DrawStrengths[3] = max(d / Total, 0);

    return DrawStrengths;
}

vec3 GetTextureCoordinate(vec3 WorldPos, int TextureIndex)
{
    return vec3(fsTextureCoords * uTextureSizes[TextureIndex], TextureIndex);
}

vec3 TerrainNormal(vec3 WorldPos, vec3 Normal)
{
    float[] DrawStrengths = TerrainBlending(WorldPos, Normal);

    vec3 Result = vec3(0);

    for (int i = 0; i < 4; i++)
    {
        float d = DrawStrengths[i];
        vec3 Normal = normalize(2.0 * texture(uNormals, GetTextureCoordinate(WorldPos, i)).xyz - 1.0f);
        Result = Result * (1 - DrawStrengths[i]) + (fsTangentMatrix * Normal) * d;
    }

    return Result;
}

vec3 ProcessDirectionalLights(vec3 Color, vec3 Normal, vec3 LightOut)
{
    vec3 Result = vec3(0.0f);

    for (int i = 0; i < uNumDirectionalLights; i++)
    {
        DirectionalLight Light = uDirectionalLights[i];

        // Ambient
        float Ambient = uTerrain.Ambient * Light.Ambient;

        // Diffuse
        vec3 Ld = -Light.Direction;
        float Df = dot(Normal, Ld);
        float Diffuse = max(Df, 0.0f) * uTerrain.Diffuse * Light.Diffuse;

        // Specular
        vec3 Hd = normalize(Ld + LightOut);
        float Sf = max(dot(Normal, Hd), 0.0);
        float Specular = pow(Sf, uTerrain.Shininess) * uTerrain.Specular * Light.Specular;
        float LightFactor = (Ambient + Diffuse + Specular) * Light.Radiance / 4.0f; // Divide by 4 to balance the intensity of directional lights

        Result += LightFactor * Color * Light.Color;
    }

    return Result;
}

vec3 ProcessPointLights(vec3 Color, vec3 Normal, vec3 LightOut)
{
    vec3 Result = vec3(0);

    for (int i = 0; i < uNumPointLights; i++)
    {
        PointLight Light = uPointLights[i];

        // Ambient
        float Ambient = uTerrain.Ambient * Light.Ambient;

        // Diffuse
        vec3 Ld = normalize(Light.Position - fsWorldPosition);
        float Df = dot(Normal, Ld);
        float Diffuse = max(Df, 0.0f) * uTerrain.Diffuse * Light.Diffuse;

        // Specular
        vec3 Hd = normalize(Ld + LightOut);
        float Sf = max(dot(Normal, Hd), 0.0f);
        float Specular = pow(Sf, uTerrain.Shininess) * uTerrain.Specular * Light.Specular;

        // Attenuation
        float Distance = length(Light.Position - fsWorldPosition);
        float Attenuation = 1.0f / (Light.Constant + Light.Linear * Distance + Light.Quadratic * (Distance * Distance));
        float LightFactor = (Ambient + Diffuse + Specular) * Attenuation;

        Result += LightFactor * Color * Light.Color;
    }

    return Result;
}

vec3 ProcessHaze(float Distance, vec3 Color)
{
    vec3 Result = Color;

    if (uHaze.Enabled)
    {
        float Factor = exp(-pow(Distance * 0.00005f * uHaze.Density, uHaze.Gradient));
        Factor = clamp(Factor, 0.0f, 1.0f);
        Result = mix(uHaze.Color * clamp(-uDirectionalLights[0].Direction.y, 0.0f, 1.0f), Color, Factor);
    }

    return Result;
}

void main()
{
    vec3 OutColor = vec3(0.0f);

    float[4] DrawStrengths = TerrainBlending(fsWorldPosition, fsNormal);

    vec3[4] TextureCoords;
    TextureCoords[0] = GetTextureCoordinate(fsWorldPosition, 0);
    TextureCoords[1] = GetTextureCoordinate(fsWorldPosition, 1);
    TextureCoords[2] = GetTextureCoordinate(fsWorldPosition, 2);
    TextureCoords[3] = GetTextureCoordinate(fsWorldPosition, 3);

    vec3 Color = vec3(0);
    vec3 Normal = vec3(0);

    float Weights[4] = GetTextureWeightsByDisplacement(TextureCoords, DrawStrengths);

    for (int i = 0; i < 4; i++)
    {
        Color += texture(uAlbedos, TextureCoords[i]).rgb * Weights[i];
        Normal += GetWorldNormal(TextureCoords[i]) * Weights[i];
    }

    vec3 LightOut = normalize(uCameraPosition - fsWorldPosition);
    float Distance = length(uCameraPosition - fsWorldPosition);

    vec3 Result = vec3(0.0f);
    Result += ProcessDirectionalLights(Color, Normal, LightOut);
    Result += ProcessPointLights(Color, Normal, LightOut);
    Result = ProcessHaze(Distance, Result);

    OutFragColor = vec4(Result, 1.0f);

    // Note that the terrain has no model matrix so its local and world coordinates are equal.
    OutFragLocalPosition = vec4(fsWorldPosition, 1.0f);
    OutFragWorldPosition = vec4(fsWorldPosition, 1.0f);
    OutFragNodeInfo = vec4(uNodeId, 0, 0, 1);

    gl_FragDepth = log2(fsLogZ) / log2(uFarPlane + 1.0);
}
