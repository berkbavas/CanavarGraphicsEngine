#version 450 core

struct Model
{
    vec3 color;
    vec3 transparencyColor;
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

struct Shadow
{
    sampler2D map;
    bool enabled;
    float bias;
    int samples;
};

uniform Haze uHaze;
uniform Model uModel;
uniform Shadow uShadow;

uniform PointLight uPointLights[8];
uniform int uNumberOfPointLights;

uniform DirectionalLight uDirectionalLights[8];
uniform int uNumberOfDirectionalLights;

uniform vec3 uCameraPosition;
uniform float uZFar;

uniform float uMeshOpacity;

uniform sampler2D uTextureBaseColor;
uniform sampler2D uTextureMetallic;
uniform sampler2D uTextureRoughness;
uniform sampler2D uTextureAmbientOcclusion;
uniform sampler2D uTextureNormal;

uniform bool uHasTextureBaseColor;
uniform bool uHasTextureMetallic;
uniform bool uHasTextureRoughness;
uniform bool uHasTextureAmbientOcclusion;
uniform bool uHasTextureNormal;

uniform int uNodeId;
uniform int uMeshId;
uniform int uSelectedMeshId;
uniform int uLightSpaceMatrixId;

uniform int uMeshSelectionEnabled;

in vec3 fsLocalPosition;
in vec3 fsWorldPosition;
in vec3 fsNormal;
in vec2 fsTextureCoords;
in mat3 fsTBN;
flat in int fsVertexId;
in float fsFlogZ;
in vec4 fsLightSpacePosition; // Position in light space
in vec3 fsColor;
in flat uint fsMask;

layout(location = 0) out vec4 OutFragColor;
layout(location = 1) out vec4 OutFragLocalPosition;
layout(location = 2) out vec4 OutFragWorldPosition;
layout(location = 3) out vec4 OutNodeInfo;

const uint NO_MASK = 0;
const uint PAINTED_MASK = 1;

const float EPSILON = 0.00001f;
const float PI = 3.14159265359;

const vec3 RED = vec3(1.0f, 0.0f, 0.0f);
const vec3 GREEN = vec3(0.0f, 1.0f, 0.0f);
const vec3 BLUE = vec3(0.0f, 0.0f, 1.0f);

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

vec3 ProcessDirectionalLights(vec3 color, vec3 normal, vec3 Lo, float shadowFactor)
{
    vec3 result = vec3(0.0f);

    for (int i = 0; i < uNumberOfDirectionalLights; i++)
    {
        DirectionalLight light = uDirectionalLights[i];

        // Ambient
        float ambient = uModel.ambient * light.ambient;

        // Diffuse
        vec3 ld = -light.direction;
        float df = dot(normal, ld);
        float diffuse = max(df, 0.0f) * uModel.diffuse * light.diffuse;

        // Specular
        vec3 hd = normalize(ld + Lo);
        float sf = max(dot(normal, hd), 0.0);
        float specular = pow(sf, uModel.shininess) * uModel.specular * light.specular;
        float lightFactor = (ambient * (1.0f - 0.5 * shadowFactor) + diffuse * (1.0f - shadowFactor) + specular * (1.0f - shadowFactor));
        result += lightFactor * color * light.color;
    }

    return result;
}

vec3 ProcessPointLights(vec3 color, vec3 normal, vec3 Lo, vec3 fragWorldPos, float shadowFactor)
{
    vec3 result = vec3(0);

    for (int i = 0; i < uNumberOfPointLights; i++)
    {
        PointLight light = uPointLights[i];

        // Ambient
        float ambient = uModel.ambient * light.ambient;

        // Diffuse
        vec3 ld = normalize(light.position - fragWorldPos);
        float df = dot(normal, ld);
        float diffuse = max(df, 0.0f) * uModel.diffuse * light.diffuse;

        // Specular
        vec3 hd = normalize(ld + Lo);
        float sf = max(dot(normal, hd), 0.0);
        float specular = pow(sf, uModel.shininess) * uModel.specular * light.specular;

        // Attenuation
        float distance = length(light.position - fragWorldPos);
        float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
        float lightFactor = (ambient * (1.0f - 0.5 * shadowFactor) + diffuse * (1.0f - shadowFactor) + specular * (1.0f - shadowFactor));
        result += lightFactor * color * light.color * attenuation;
    }

    return result;
}

vec3 ProcessHaze(float distance, vec3 fragWorldPos, vec3 subjectColor)
{
    vec3 result = subjectColor;

    if (uHaze.enabled)
    {
        float factor = exp(-pow(distance * 0.00005f * uHaze.density, uHaze.gradient));
        factor = clamp(factor, 0.0f, 1.0f);
        result = mix(uHaze.color * clamp(-uDirectionalLights[0].direction.y, 0.0f, 1.0f), subjectColor, factor);
    }

    return result;
}

vec3 ProcessDirectionalLightsPbr(vec3 albedo, float metallic, float roughness, float ambientOcclusion, vec3 N /* Normal */, vec3 Lo, float shadowFactor)
{
    vec3 F0 = mix(vec3(0.04f), albedo, metallic);

    vec3 result = vec3(0.0);

    for (int i = 0; i < uNumberOfDirectionalLights; ++i)
    {
        DirectionalLight light = uDirectionalLights[i];

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
    vec3 color = ambient * (1 - 0.5 * shadowFactor) + result * (1 - shadowFactor);

    return color;
}

vec3 ProcessPointLightsPbr(vec3 albedo, float metallic, float roughness, float ambientOcclusion, vec3 N /* Normal */, vec3 Lo, vec3 fragWorldPos, float shadowFactor)
{
    vec3 F0 = mix(vec3(0.04f), albedo, metallic);

    vec3 result = vec3(0.0);

    for (int i = 0; i < uNumberOfPointLights; ++i)
    {
        PointLight light = uPointLights[i];

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
    vec3 color = ambient * (1 - 0.5 * shadowFactor) + result * (1 - shadowFactor);

    return color;
}

float CalculateMetallic()
{
    float metallic;

    if (uHasTextureMetallic)
    {
        metallic = texture(uTextureMetallic, fsTextureCoords).r;
    }
    else
    {
        metallic = uModel.metallic;
    }

    return metallic;
}

float CalculateRoughness()
{
    float roughness;

    if (uHasTextureRoughness)
    {
        roughness = texture(uTextureRoughness, fsTextureCoords).r;
    }
    else
    {
        roughness = uModel.roughness;
    }

    return roughness;
}

float CalculateAmbientOcclusion()
{
    float ao;

    if (uHasTextureAmbientOcclusion)
    {
        ao = texture(uTextureAmbientOcclusion, fsTextureCoords).r;
    }
    else
    {
        ao = uModel.ambientOcclusion;
    }

    return ao;
}

vec3 CalculateNormal()
{
    vec3 normal;

    if (uHasTextureNormal)
    {
        normal = texture(uTextureNormal, fsTextureCoords).xyz;
        normal = 2.0 * normal - 1.0;
        normal = normalize(fsTBN * normal);
    }
    else
    {
        normal = fsNormal;
    }

    return normal;
}

vec3 CalculateColor(float opacity)
{
    vec3 color;

    if (uModel.useModelColor)
    {
        if (opacity < 1.0f)
        {
            color = uModel.transparencyColor;
        }
        else
        {
            color = uModel.color;
        }
    }
    else
    {
        if (uHasTextureBaseColor)
        {
            color = texture(uTextureBaseColor, fsTextureCoords).rgb;
        }
        else
        {
            if (opacity < 1.0f)
            {
                color = uModel.transparencyColor;
            }
            else
            {
                color = uModel.color;
            }
        }
    }

    if (fsMask == PAINTED_MASK)
    {
        color = fsColor;
    }

    if (uMeshSelectionEnabled == 1)
    {
        if (uSelectedMeshId == uMeshId)
        {
            color = mix(color, RED, 0.5f);
        }
    }

    return color;
}

// Reference: https://learnopengl.com/code_viewer_gh.php?code=src/5.advanced_lighting/3.1.3.shadow_mapping/3.1.3.shadow_mapping.fs
float CalculateShadow()
{
    float result = 0.0f;

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
    }

    return result;
}

float CalculateOpacity()
{
    float opacity;

    if (uHasTextureBaseColor)
    {
        opacity = texture(uTextureBaseColor, fsTextureCoords).a;
    }
    else
    {
        opacity = uMeshOpacity;
    }

    return opacity;
}

void main()
{
    float shadowFactor = CalculateShadow();
    float opacity = CalculateOpacity();
    vec3 normal = CalculateNormal();
    vec3 color = CalculateColor(opacity);
    vec3 Lo = normalize(uCameraPosition - fsWorldPosition);
    float distance = length(uCameraPosition - fsWorldPosition);

    // Process
    vec3 result = vec3(0.0f);

    if (uModel.shadingMode == PBR_SHADING)
    {
        float metallic = CalculateMetallic();
        float roughness = CalculateRoughness();
        float ambientOcclusion = CalculateAmbientOcclusion();

        result += ProcessDirectionalLightsPbr(color, metallic, roughness, ambientOcclusion, normal, Lo, shadowFactor);
        result += ProcessPointLightsPbr(color, metallic, roughness, ambientOcclusion, normal, Lo, fsWorldPosition, shadowFactor);
    }
    else if (uModel.shadingMode == PHONG_SHADING)
    {
        result += ProcessDirectionalLights(color, normal, Lo, shadowFactor);
        result += ProcessPointLights(color, normal, Lo, fsWorldPosition, shadowFactor);
    }

    // Final
    result = ProcessHaze(distance, fsWorldPosition, result);

    OutFragColor = vec4(result, opacity);

    // Fragment position
    OutFragLocalPosition = vec4(fsLocalPosition, 1.0f);
    OutFragWorldPosition = vec4(fsWorldPosition, 1.0f);

    // Node Info
    OutNodeInfo = vec4(float(uNodeId), float(uMeshId), float(gl_PrimitiveID), 1.0f);

    gl_FragDepth = log2(fsFlogZ) / log2(uZFar + 1.0);
}
