#version 430 core

struct Model
{
    vec3 color;
    float ambient;
    float diffuse;
    float specular;
    float shininess;
    float metallic;
    float roughness;
    float ambientOcclusion;
    bool useModelColor;
    int shadingMode;
};

struct DirectionalLight
{
    vec3 color;
    vec3 direction;
    float ambient;
    float diffuse;
    float specular;
    float radiance;
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

struct Haze
{
    vec3 color;
    float density;
    float gradient;
    bool enabled;
};

uniform Haze haze;
uniform Model model;

uniform PointLight pointLights[8];
uniform int numberOfPointLights;

uniform DirectionalLight directionalLights[8];
uniform int numberOfDirectionalLights;

uniform vec3 cameraPosition;
uniform float zFar;

uniform sampler2D textureBaseColor;
uniform sampler2D textureMetallic;
uniform sampler2D textureRoughness;
uniform sampler2D textureAmbientOcclusion;
uniform sampler2D textureNormal;

uniform bool hasTextureBaseColor;
uniform bool hasTextureMetallic;
uniform bool hasTextureRoughness;
uniform bool hasTextureAmbientOcclusion;
uniform bool hasTextureNormal;

uniform float nodeId;
uniform float meshId;

in vec3 fsLocalPosition;
in vec3 fsWorldPosition;
in vec3 fsNormal;
in vec2 fsTextureCoords;
in mat3 fsTBN;
flat in int fsVertexId;
in float fsFlogZ;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragLocalPosition;
layout(location = 2) out vec4 fragWorldPosition;
layout(location = 3) out vec4 nodeInfo;
layout(location = 4) out vec4 outDistance;

const float EPSILON = 0.00001f;

const float PI = 3.14159265359;

const int PBR_SHADING = 0;
const int PHONG_SHADING = 1;

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return a2 / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;

    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec3 CalculateNormal()
{
    vec3 normal;

    if (hasTextureNormal)
    {
        normal = texture(textureNormal, fsTextureCoords).xyz;
        normal = 2.0 * normal - 1.0;
        normal = normalize(fsTBN * normal);
    }
    else
    {
        normal = fsNormal;
    }

    return normal;
}

vec3 ProcessDirectionalLights(vec3 color, vec3 normal, vec3 Lo)
{
    vec3 result = vec3(0.0f);

    for (int i = 0; i < numberOfDirectionalLights; i++)
    {
        DirectionalLight light = directionalLights[i];

        // Ambient
        float ambient = model.ambient * light.ambient;

        // Diffuse
        vec3 ld = -light.direction;
        float df = dot(normal, ld);
        float diffuse = max(df, 0.0f) * model.diffuse * light.diffuse;

        // Specular
        vec3 hd = normalize(ld + Lo);
        float sf = max(dot(normal, hd), 0.0);
        float specular = pow(sf, model.shininess) * model.specular * light.specular;

        result += (ambient + diffuse + specular) * color * light.color;
    }

    return result;
}

vec3 ProcessPointLights(vec3 color, vec3 normal, vec3 Lo, vec3 fragWorldPos)
{
    vec3 result = vec3(0);

    for (int i = 0; i < numberOfPointLights; i++)
    {
        PointLight light = pointLights[i];

        // Ambient
        float ambient = model.ambient * light.ambient;

        // Diffuse
        vec3 ld = normalize(light.position - fragWorldPos);
        float df = dot(normal, ld);
        float diffuse = max(df, 0.0f) * model.diffuse * light.diffuse;

        // Specular
        vec3 hd = normalize(ld + Lo);
        float sf = max(dot(normal, hd), 0.0);
        float specular = pow(sf, model.shininess) * model.specular * light.specular;

        // Attenuation
        float distance = length(light.position - fragWorldPos);
        float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

        result += (ambient + diffuse + specular) * color * light.color * attenuation;
    }

    return result;
}

vec3 ProcessHaze(float distance, vec3 fragWorldPos, vec3 subjectColor)
{
    vec3 result = subjectColor;

    if (haze.enabled)
    {
        float factor = exp(-pow(distance * 0.00005f * haze.density, haze.gradient));
        factor = clamp(factor, 0.0f, 1.0f);
        result = mix(haze.color * clamp(directionalLights[0].direction.y, 0.0f, 1.0f), subjectColor, factor);
    }

    return result;
}

vec3 CalculateColor()
{
    vec3 color;

    if (model.useModelColor)
    {
        color = model.color;
    }
    else
    {
        if (hasTextureBaseColor)
        {
            color = texture(textureBaseColor, fsTextureCoords).rgb;
        }
        else
        {
            color = model.color;
        }
    }

    return color;
}

vec3 ProcessDirectionalLightsPbr(vec3 albedo, float metallic, float roughness, float ambientOcclusion, vec3 N /* Normal */, vec3 Lo)
{
    vec3 F0 = mix(vec3(0.04f), albedo, metallic);

    vec3 result = vec3(0.0);

    for (int i = 0; i < numberOfDirectionalLights; ++i)
    {
        DirectionalLight light = directionalLights[i];

        vec3 Li = -light.direction;
        vec3 H = normalize(Lo + Li);
        vec3 radiance = light.color * light.radiance;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, Lo, Li, roughness);
        vec3 F = FresnelSchlick(max(dot(H, Lo), 0.0f), F0);

        vec3 nominator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, Lo), 0.0f) * max(dot(N, Li), 0.0f) + 0.001f;
        vec3 specular = nominator / denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0f) - kS;
        kD *= 1.0f - metallic;

        float NdotL = max(dot(N, Li), 0.0f);
        result += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    // Ambient (no IBL, just constant AO term)
    vec3 ambient = vec3(0.03f) * albedo * ambientOcclusion;
    vec3 color = ambient + result;

    return color;
}

vec3 ProcessPointLightsPbr(vec3 albedo, float metallic, float roughness, float ambientOcclusion, vec3 N /* Normal */, vec3 Lo, vec3 fragWorldPos)
{
    vec3 F0 = mix(vec3(0.04f), albedo, metallic);

    vec3 result = vec3(0.0);

    for (int i = 0; i < numberOfPointLights; ++i)
    {
        PointLight light = pointLights[i];

        vec3 Li = normalize(light.position - fragWorldPos);
        vec3 H = normalize(Lo + Li);
        float distance = length(light.position - fragWorldPos);
        float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
        vec3 radiance = light.color * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, Lo, Li, roughness);
        vec3 F = FresnelSchlick(max(dot(H, Lo), 0.0f), F0);

        vec3 nominator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, Lo), 0.0f) * max(dot(N, Li), 0.0f) + 0.001f;
        vec3 specular = nominator / denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0f) - kS;
        kD *= 1.0f - metallic;

        float NdotL = max(dot(N, Li), 0.0f);
        result += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    // Ambient (no IBL, just constant AO term)
    vec3 ambient = vec3(0.03f) * albedo * ambientOcclusion;
    vec3 color = ambient + result;

    return color;
}

float CalculateMetallic()
{
    float metallic;

    if (hasTextureMetallic)
    {
        vec3 rgb = texture(textureMetallic, fsTextureCoords).rgb;
        metallic = (rgb.r + rgb.g + rgb.b) / 3.0f;
    }
    else
    {
        metallic = model.metallic;
    }

    return metallic;
}

float CalculateRoughness()
{
    float roughness;

    if (hasTextureRoughness)
    {
        vec3 rgb = texture(textureRoughness, fsTextureCoords).rgb;
        roughness = (rgb.r + rgb.g + rgb.b) / 3.0f;
    }
    else
    {
        roughness = model.roughness;
    }

    return roughness;
}

float CalculateAmbientOcclusion()
{
    float ao;

    if (hasTextureAmbientOcclusion)
    {
        vec3 rgb = texture(textureAmbientOcclusion, fsTextureCoords).rgb;
        ao = (rgb.r + rgb.g + rgb.b) / 3.0f;
    }
    else
    {
        ao = model.ambientOcclusion;
    }

    return ao;
}

void main()
{
    vec3 normal = CalculateNormal();
    vec3 color = CalculateColor();
    vec3 Lo = normalize(cameraPosition - fsWorldPosition);
    float distance = length(cameraPosition - fsWorldPosition);

    // Process
    vec3 result = vec3(0.0f);

    if (model.shadingMode == PBR_SHADING)
    {
        float metallic = CalculateMetallic();
        float roughness = CalculateRoughness();
        float ambientOcclusion = CalculateAmbientOcclusion();

        result += ProcessDirectionalLightsPbr(color, metallic, roughness, ambientOcclusion, normal, Lo);
        result += ProcessPointLightsPbr(color, metallic, roughness, ambientOcclusion, normal, Lo, fsWorldPosition);
    }
    else if (model.shadingMode == PHONG_SHADING)
    {
        result += ProcessDirectionalLights(color, normal, Lo);
        result += ProcessPointLights(color, normal, Lo, fsWorldPosition);
    }

    // Final
    result = ProcessHaze(distance, fsWorldPosition, result);

    fragColor = vec4(result, 1.0f);

    // Fragment position
    fragLocalPosition = vec4(fsLocalPosition, 1.0f);
    fragWorldPosition = vec4(fsWorldPosition, 1.0f);

    // Node Info
    nodeInfo = vec4(nodeId, meshId, float(gl_PrimitiveID), 1.0f);

    // Distance
    outDistance = vec4(distance, 0.0f, 0.0f, 1.0f);

    gl_FragDepth = log2(fsFlogZ) / log2(zFar + 1.0);
}
