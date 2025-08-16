#version 450 core

// Taken from https://github.com/AntonHakansson/procedural-terrain

#define PI 3.14159265359

struct Terrain
{
    float ambient;
    float diffuse;
    float specular;
    float shininess;
};

struct Haze
{
    bool enabled;
    vec3 color;
    float density;
    float gradient;
};

struct Noise
{
    int octaves;
    float amplitude;
    float frequency;
    float persistence;
    float lacunarity;
};

struct DirectionalLight
{
    vec3 color;
    vec3 direction;
    float ambient;
    float diffuse;
    float specular;
};

struct PointLight
{
    vec3 color;
    vec3 position;
    float ambient;
    float diffuse;
    float specular;
    float constant;
    float linear;
    float quadratic;
};

struct Shadow
{
    sampler2D map;
    bool enabled;
    float bias;
    int samples;
};

layout(binding = 0) uniform sampler2DArray uAlbedos;
layout(binding = 1) uniform sampler2DArray uNormals;
layout(binding = 2) uniform sampler2DArray uDisplacement;

in vec3 fsWorldPosition;
in vec3 fsViewSpacePosition;
in vec3 fsViewSpaceNormal;
in vec2 fsTextureCoords;
in vec3 fsNormal;
in vec3 fsTangent;
in vec3 fsBitangent;
in mat3 fsTangentMatrix;
in float fsLogZ;
in vec4 fsLightSpacePosition;
in float fsDepth;

layout(location = 0) out vec4 OutFragColor;
layout(location = 1) out vec4 OutFragLocalPosition;
layout(location = 2) out vec4 OutFragWorldPosition;
layout(location = 3) out vec4 OutFragNodeInfo;
layout(location = 4) out vec4 OutFragDepth;

uniform vec3 uCameraPosition;
uniform float uZFar;

uniform float uTextureStartHeights[4];
uniform float uTextureBlends[4];
uniform float uTextureSizes[4];
uniform float uTextureDisplacementWeights[4];

uniform Noise uNoise;
uniform Haze uHaze;
uniform Terrain uTerrain;
uniform Shadow uShadow;

uniform PointLight uPointLights[8];
uniform int uNumberOfPointLights;

uniform DirectionalLight uDirectionalLights[8]; // First element is the Sun
uniform int uNumberOfDirectionalLights;

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
    vec3 tangentNormal = normalize(2.0 * texture(uNormals, textureCoords).xyz - 1.0f);
    return fsTangentMatrix * tangentNormal;
}

float[4] GetTextureWeightsByDisplacement(vec3[4] t, float[4] a)
{
    const float depth = 0.1f;

    float disp[t.length];
    for (int i = 0; i < t.length; i++)
    {
        disp[i] = texture(uDisplacement, t[i]).r * uTextureDisplacementWeights[i];
    }

    float ma = 0;
    for (int i = 0; i < t.length; i++)
    {
        ma = max(ma, disp[i] + a[i]);
    }
    ma -= depth;

    float total_w = 0;
    float[t.length] w;
    for (int i = 0; i < t.length; i++)
    {
        w[i] = max(disp[i] + a[i] - ma, 0);
        total_w += w[i];
    }

    // normalize
    for (int i = 0; i < t.length; i++)
    {
        w[i] /= total_w;
    }

    return w;
}

float[4] TerrainBlending(vec3 worldPos, vec3 normal)
{
    float height = worldPos.y;

    // A completely flat terrain has slope=0
    float slope = max(1 - dot(normal, vec3(0, 1, 0)), 0.0f);

    // For each fragment we compute how much each texture contributes depending on height and slope
    float drawStrengths[4];

    float sandGrassHeight = uTextureStartHeights[0];
    float grassRockHeight = uTextureStartHeights[1];
    float rockSnowHeight = uTextureStartHeights[2];

    float grassFalloff = uTextureBlends[1];
    float rockFalloff = uTextureBlends[2];
    float snowFalloff = uTextureBlends[3];

    float a, b, c, d;
    {
        b = InverseLerpClamped(sandGrassHeight - grassFalloff / 2, sandGrassHeight + grassFalloff / 2, height);
    }
    {
        c = InverseLerpClamped(grassRockHeight - rockFalloff / 2, grassRockHeight + rockFalloff / 2, height);
    }

    float b_in, c_in, d_in;
    b_in = InverseLerpClamped(sandGrassHeight - grassFalloff / 2, sandGrassHeight + grassFalloff / 2, height);
    c_in = InverseLerpClamped(grassRockHeight - rockFalloff / 2, grassRockHeight + rockFalloff / 2, height);
    d_in = InverseLerpClamped(rockSnowHeight - snowFalloff / 2, rockSnowHeight + snowFalloff / 2, height);

    a = 1 - b_in;
    b *= 1 - c_in;
    c *= 1 - d_in;
    d = d_in;

    b *= 1 - InverseLerpClamped(0.1f, 1.0f, slope);
    d *= 1 - InverseLerpClamped(0.1f, 1.0f, slope);

    float tot = a + b + c + d;
    drawStrengths[0] = max(a / tot, 0);
    drawStrengths[1] = max(b / tot, 0);
    drawStrengths[2] = max(c / tot, 0);
    drawStrengths[3] = max(d / tot, 0);

    return drawStrengths;
}

vec3 GetTextureCoordinate(vec3 worldPos, int textureIndex)
{
    return vec3(fsTextureCoords * uTextureSizes[textureIndex], textureIndex);
}

vec3 TerrainNormal(vec3 worldPos, vec3 normal)
{
    float[] drawStrengths = TerrainBlending(worldPos, normal);

    vec3 result = vec3(0);

    for (int i = 0; i < 4; i++)
    {
        float d = drawStrengths[i];
        vec3 normal = normalize(2.0 * texture(uNormals, GetTextureCoordinate(worldPos, i)).xyz - 1.0f);
        result = result * (1 - drawStrengths[i]) + (fsTangentMatrix * normal) * d;
    }

    return result;
}

vec3 ProcessDirectionalLights(vec3 color, vec3 normal, vec3 lightOut, float shadow)
{
    vec3 result = vec3(0.0f);

    for (int i = 0; i < uNumberOfDirectionalLights; i++)
    {
        DirectionalLight light = uDirectionalLights[i];

        // Ambient
        float ambient = uTerrain.ambient * light.ambient;

        // Diffuse
        vec3 ld = -light.direction;
        float df = dot(normal, ld);
        float diffuse = max(df, 0.0f) * uTerrain.diffuse * light.diffuse;

        // Specular
        vec3 hd = normalize(ld + lightOut);
        float sf = max(dot(normal, hd), 0.0);
        float specular = pow(sf, uTerrain.shininess) * uTerrain.specular * light.specular;
        float factor = (ambient * (1 - 0.5 * shadow) + diffuse * (1 - shadow) + specular * (1 - shadow));

        result += factor * color * light.color;
    }

    return result;
}

vec3 ProcessPointLights(vec3 color, vec3 normal, vec3 lightOut, vec3 worldPos, float shadow)
{
    vec3 result = vec3(0);

    for (int i = 0; i < uNumberOfPointLights; i++)
    {
        PointLight light = uPointLights[i];

        // Ambient
        float ambient = uTerrain.ambient * light.ambient;

        // Diffuse
        vec3 ld = normalize(light.position - worldPos);
        float df = dot(normal, ld);
        float diffuse = max(df, 0.0f) * uTerrain.diffuse * light.diffuse;

        // Specular
        vec3 hd = normalize(ld + lightOut);
        float sf = max(dot(normal, hd), 0.0f);
        float specular = pow(sf, uTerrain.shininess) * uTerrain.specular * light.specular;

        // Attenuation
        float distance = length(light.position - worldPos);
        float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
        float factor = (ambient * (1 - 0.5 * shadow) + diffuse * (1 - shadow) + specular * (1 - shadow));

        result += factor * color * light.color * attenuation;
    }

    return result;
}

vec3 ProcessHaze(float distance, vec3 color)
{
    vec3 result = color;

    if (uHaze.enabled)
    {
        float factor = exp(-pow(distance * 0.00005f * uHaze.density, uHaze.gradient));
        factor = clamp(factor, 0.0f, 1.0f);
        result = mix(uHaze.color * clamp(-uDirectionalLights[0].direction.y, 0.0f, 1.0f), color, factor);
    }

    return result;
}

// Reference: https://learnopengl.com/code_viewer_gh.php?code=src/5.advanced_lighting/3.1.3.shadow_mapping/3.1.3.shadow_mapping.fs
float CalculateShadow()
{
    if (uShadow.enabled)
    {
        // perform perspective divide
        vec3 projCoords = fsLightSpacePosition.xyz / fsLightSpacePosition.w;

        // transform to [0,1] range
        projCoords = projCoords * 0.5 + 0.5;

        // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
        float closestDepth = texture(uShadow.map, projCoords.xy).r;

        // get depth of current fragment from light's perspective
        float currentDepth = projCoords.z;

        vec2 texelSize = 1.0 / textureSize(uShadow.map, 0);

        int samples = uShadow.samples;
        float result = 0.0f;

        for (int x = -samples; x <= samples; ++x)
        {
            for (int y = -samples; y <= samples; ++y)
            {
                float pcfDepth = texture(uShadow.map, projCoords.xy + vec2(x, y) * texelSize).r;
                result += currentDepth - uShadow.bias > pcfDepth ? 1.0 : 0.0;
            }
        }

        result /= pow(2 * samples + 1, 2);

        // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
        if (projCoords.z > 1.0)
        {
            result = 0.0;
        }

        return result;
    }

    return 0.0f; // No shadow if shadow mapping is disabled
}

void main()
{
    vec3 outColor = vec3(0.0f);

    float[4] drawStrengths = TerrainBlending(fsWorldPosition, fsNormal);

    vec3[4] textureCoords;
    textureCoords[0] = GetTextureCoordinate(fsWorldPosition, 0);
    textureCoords[1] = GetTextureCoordinate(fsWorldPosition, 1);
    textureCoords[2] = GetTextureCoordinate(fsWorldPosition, 2);
    textureCoords[3] = GetTextureCoordinate(fsWorldPosition, 3);

    vec3 color = vec3(0);
    vec3 normal = vec3(0);

    float weights[4] = GetTextureWeightsByDisplacement(textureCoords, drawStrengths);

    for (int i = 0; i < weights.length; i++)
    {
        color += texture(uAlbedos, textureCoords[i]).rgb * weights[i];
        normal += GetWorldNormal(textureCoords[i]) * weights[i];
    }

    float shadow = CalculateShadow();

    vec3 lightOut = normalize(uCameraPosition - fsWorldPosition);
    float distance = length(uCameraPosition - fsWorldPosition);

    vec3 result = vec3(0.0f);
    result += ProcessDirectionalLights(color, normal, lightOut, shadow);
    result += ProcessPointLights(color, normal, lightOut, fsWorldPosition, shadow);
    result = ProcessHaze(distance, result);

    // Note that the terrain has no model matrix so its local and world coordinates are equal.
    OutFragLocalPosition = vec4(fsWorldPosition, 1.0f);
    OutFragWorldPosition = vec4(fsWorldPosition, 1.0f);

    OutFragNodeInfo = vec4(uNodeId, 0, 0, 1);
    OutFragColor = vec4(result, 1.0f);
    OutFragDepth = vec4(fsDepth, fsDepth, fsDepth, 1.0f);
    gl_FragDepth = log2(fsLogZ) / log2(uZFar + 1.0);
}
