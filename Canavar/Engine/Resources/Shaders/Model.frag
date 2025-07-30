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
    bool useColor;
    bool invertNormals;
    int shadingMode;
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
    float constant;
    float linear;
    float quadratic;
    float ambient;
    float diffuse;
    float specular;
};

struct Haze
{
    vec3 color;
    float density;
    float gradient;
    bool enabled;
};

struct Shadow
{
    sampler2D map;
    float bias;
    int samples;
    bool enabled;
};

uniform Haze haze;
uniform Model model;
uniform Shadow shadow;

uniform PointLight pointLights[8];
uniform int numberOfPointLights;

uniform DirectionalLight directionalLights[8];
uniform int numberOfDirectionalLights;

uniform vec3 cameraPosition;
uniform vec3 sunDirection;

uniform float zFar;

// PBR
uniform sampler2D textureBaseColor;
uniform sampler2D textureNormal;
uniform sampler2D textureRoughness;
uniform sampler2D textureMetallic;
uniform sampler2D textureAmbientOcclusion;

uniform bool hasTextureBaseColor;
uniform bool hasTextureNormal;
uniform bool hasTextureRoughness;
uniform bool hasTextureMetallic;
uniform bool hasTextureAmbientOcclusion;

uniform float nodeId;
uniform float meshId;

in vec4 fsLocalPosition;
in vec4 fsWorldPosition;
in vec3 fsNormal;
in vec2 fsTextureCoords;
in mat3 fsTBN;
in vec4 fsFragPosLightSpace;
in vec4 fsVertexColor;
flat in int fsVertexId;
in float fsFlogZ;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragLocalPosition;
layout(location = 2) out vec4 fragWorldPosition;
layout(location = 3) out vec4 nodeInfo;
layout(location = 4) out vec4 outDistance;

const float PI = 3.14159265359;
const int PBR_SHADING = 0;
const int PHONG_SHADING = 1;

const vec3 F_DIELECTRIC = vec3(0.04);
const float EPSILON = 0.00001f;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    return a2 / (PI * denom * denom);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    return GeometrySchlickGGX(max(dot(N, V), 0.0), roughness) * GeometrySchlickGGX(max(dot(N, L), 0.0), roughness);
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 CalculateNormal()
{
    vec3 normal;

    if (hasTextureNormal)
    {
        normal = texture(textureNormal, fsTextureCoords).rgb;
        normal = 2.0 * normal - 1.0;
        normal = normalize(fsTBN * normal);
    }
    else
    {
        normal = fsNormal;
    }

    if (model.invertNormals)
    {
        normal = -normal;
    }

    return normal;
}

vec3 ProcessDirectionalLights(vec3 color, vec3 normal, vec3 viewDirection)
{
    vec3 result = vec3(0.0f);

    for (int i = 0; i < numberOfDirectionalLights; i++)
    {
        DirectionalLight light = directionalLights[i];

        // Ambient
        float ambient = model.ambient * light.ambient;

        // Diffuse
        float diffuse = max(dot(normal, -light.direction), 0.0) * model.diffuse * light.diffuse;

        // Specular
        vec3 reflectDirection = reflect(light.direction, normal);
        float specular = pow(max(dot(viewDirection, reflectDirection), 0.0), model.shininess) * model.specular * light.specular;

        result += (ambient + diffuse + specular) * color * light.color;
    }

    return result;
}

vec3 ProcessPointLights(vec3 color, vec3 normal, vec3 viewDirection, vec3 fragWorldPos)
{
    vec3 result = vec3(0);

    for (int i = 0; i < numberOfPointLights; i++)
    {
        PointLight light = pointLights[i];

        // Ambient
        float ambient = model.ambient * light.ambient;

        // Diffuse
        vec3 lightDirection = normalize(light.position - fragWorldPos);
        float diffuse = max(dot(normal, lightDirection), 0.0) * model.diffuse * light.diffuse;

        // Specular
        vec3 halfwayDirection = normalize(lightDirection + viewDirection);
        float specular = pow(max(dot(normal, halfwayDirection), 0.0), model.shininess) * model.specular * light.specular;

        // Attenuation
        float distance = length(light.position - fragWorldPos);
        float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

        result += (ambient + diffuse + specular) * color * light.color * attenuation;
    }

    return result;
}

vec3 ProcessPointLightsPbr(vec3 albedo, float metallic, float roughness, float ambientOcclusion, vec3 N /* Normal */, vec3 V /* View Direction*/, vec3 fragWorldPos)
{
    vec3 result = vec3(0.0f);

    for (int i = 0; i < numberOfPointLights; ++i)
    {
        PointLight light = pointLights[i];
        vec3 L = normalize(light.position - fragWorldPos);
        vec3 H = normalize(V + L);
        float distance = length(light.position - fragWorldPos);
        float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
        vec3 radiance = light.color * attenuation;

        // Reflectance at normal incidence
        vec3 F0 = vec3(0.04f);
        F0 = mix(F0, albedo, metallic);

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = FresnelSchlick(max(dot(H, V), 0.0f), F0);

        vec3 nominator = NDF * G * F;
        float denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 0.001f;
        vec3 specular = nominator / denominator;

        vec3 kS = F;
        vec3 kD = (1.0 - kS) * (1.0 - metallic);
        float NdotL = max(dot(N, L), 0.0);
        vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;

        vec3 ambient = vec3(0.03f) * albedo * ambientOcclusion;

        result += ambient + Lo;
    }

    return result;
}

vec3 ProcessDirectionalLightsPbr(vec3 albedo, float metallic, float roughness, float ambientOcclusion, vec3 N /* Normal */, vec3 V /* View Direction*/, vec3 fragWorldPos)
{
    vec3 result = vec3(0.0f);

    for (int i = 0; i < numberOfDirectionalLights; ++i)
    {
        DirectionalLight light = directionalLights[i];
        vec3 L = -light.direction;
        vec3 H = normalize(V + L);
        vec3 radiance = light.color;

        // Reflectance at normal incidence
        vec3 F0 = vec3(0.04f);
        F0 = mix(F0, albedo, metallic);

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = FresnelSchlick(max(dot(H, V), 0.0f), F0);

        vec3 nominator = NDF * G * F;
        float denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 0.001f;
        vec3 specular = nominator / denominator;

        vec3 kS = F;
        vec3 kD = (1.0 - kS) * (1.0 - metallic);
        float NdotL = max(dot(N, L), 0.0);
        vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;

        vec3 ambient = vec3(0.03f) * albedo * ambientOcclusion;

        result += ambient + Lo;
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

// Reference: https://learnopengl.com/code_viewer_gh.php?code=src/5.advanced_lighting/3.1.3.shadow_mapping/3.1.3.shadow_mapping.fs
float CalculateShadow()
{
    float factor = 0.0f;

    if (shadow.enabled)
    {
        // perform perspective divide
        vec3 projCoords = fsFragPosLightSpace.xyz / fsFragPosLightSpace.w;

        // transform to [0,1] range
        projCoords = projCoords * 0.5 + 0.5;

        // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
        float closestDepth = texture(shadow.map, projCoords.xy).r;

        // get depth of current fragment from light's perspective
        float currentDepth = projCoords.z;

        vec2 texelSize = 1.0 / textureSize(shadow.map, 0);

        int samples = shadow.samples;

        for (int x = -samples; x <= samples; ++x)
        {
            for (int y = -samples; y <= samples; ++y)
            {
                float pcfDepth = texture(shadow.map, projCoords.xy + vec2(x, y) * texelSize).r;
                factor += currentDepth - shadow.bias > pcfDepth ? 1.0 : 0.0;
            }
        }

        factor /= pow(2 * samples + 1, 2);

        // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
        if (projCoords.z > 1.0)
        {
            factor = 0.0;
        }
    }

    return factor;
}

vec3 CalculateColor(float shadow)
{
    const float lit = 1 - shadow;

    vec3 color;

    if (model.useColor)
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

    return color * (0.5 + 0.5 * lit);
}

void main()
{
    vec3 normal = CalculateNormal();
    float shadowFactor = CalculateShadow();
    vec3 color = CalculateColor(shadowFactor);
    vec3 viewDirection = normalize(cameraPosition - fsWorldPosition.xyz);
    float distance = length(cameraPosition - fsWorldPosition.xyz);

    // Process
    vec3 result = vec3(0.0f);

    if (model.shadingMode == PBR_SHADING)
    {
        float metallic = hasTextureMetallic ? texture(textureMetallic, fsTextureCoords).r : model.metallic;
        float roughness = hasTextureRoughness ? texture(textureRoughness, fsTextureCoords).r : model.roughness;
        float ambientOcclusion = hasTextureAmbientOcclusion ? texture(textureAmbientOcclusion, fsTextureCoords).r : model.ambientOcclusion;

        result += ProcessDirectionalLightsPbr(color, metallic, roughness, ambientOcclusion, normal, viewDirection, fsWorldPosition.xyz);
        result += ProcessPointLightsPbr(color, metallic, roughness, ambientOcclusion, normal, viewDirection, fsWorldPosition.xyz);
    }
    else if (model.shadingMode == PHONG_SHADING)
    {
        result += ProcessDirectionalLights(color, normal, viewDirection);
        result += ProcessPointLights(color, normal, viewDirection, fsWorldPosition.xyz);
    }

    // Final
    result = ProcessHaze(distance, fsWorldPosition.xyz, result);

    fragColor = vec4(result, 1.0f);

    // Fragment position
    fragLocalPosition = vec4(fsLocalPosition.xyz, 1.0f);
    fragWorldPosition = fsWorldPosition;

    // Node Info
    nodeInfo = vec4(nodeId, meshId, float(gl_PrimitiveID), 1.0f);

    // Distance
    outDistance = vec4(distance, 0.0f, 0.0f, 1.0f);

    gl_FragDepth = log2(fsFlogZ) / log2(zFar + 1.0);
}
