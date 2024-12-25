#version 430 core

struct Model
{
    vec4 color;
    float ambient;
    float diffuse;
    float specular;
    float shininess;
    bool useColor;
    bool invertNormals;
};

struct PbrParameters
{
    vec3 baseColor;
    float metallic;
    float roughness;
};

struct DirectionalLight
{
    vec3 direction;
    vec4 color;
    float ambient;
    float diffuse;
    float specular;
};

struct PointLight
{
    vec4 color;
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
    bool enabled;
    vec3 color;
    float density;
    float gradient;
};

struct Color
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

struct Shadow
{
    sampler2D map;
    bool enabled;
    float bias;
    int samples;
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

// PBR
uniform sampler2D textureBaseColor;
uniform sampler2D textureRoughness;
uniform sampler2D textureMetallic;

// Phong Shading
uniform sampler2D textureAmbient;
uniform sampler2D textureDiffuse;
uniform sampler2D textureSpecular;

uniform sampler2D textureNormal;
uniform bool useTextureNormal;

uniform bool hasAnyColorTexture;

// WTF? float?
uniform float nodeId;
uniform float meshId;

uniform float zFar;

in vec4 fsLocalPosition;
in vec4 fsWorldPosition;
in vec3 fsNormal;
in vec2 fsTextureCoords;
in mat3 fsTBN;
in vec4 fsFragPosLightSpace;
in vec4 fsVertexColor;
flat in int fsVertexId;
in float fsflogz;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragLocalPosition;
layout(location = 2) out vec4 fragWorldPosition;
layout(location = 3) out vec4 nodeInfo;
layout(location = 4) out vec4 outDistance;

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 getNormal()
{
    if (useTextureNormal)
    {
        vec3 normal = texture(textureNormal, fsTextureCoords).rgb;
        normal = 2.0 * normal - 1.0;
        normal = normalize(fsTBN * normal);
        return normal;
    }
    else
    {
        return fsNormal;
    }
}

vec4 processDirectionalLights(Color color, vec3 normal, vec3 viewDirection)
{
    vec4 result = vec4(0);

    for (int i = 0; i < numberOfDirectionalLights; i++)
    {
        DirectionalLight light = directionalLights[i];

        // Ambient
        float ambient = light.ambient;

        // Diffuse
        float diffuse = max(dot(normal, -light.direction), 0.0) * light.diffuse;

        // Specular
        vec3 reflectDirection = reflect(light.direction, normal);
        float specular = pow(max(dot(viewDirection, reflectDirection), 0.0), model.shininess) * light.specular;

        result += (ambient * color.ambient + diffuse * color.diffuse + specular * color.specular) * light.color;
    }

    return result;
}

vec4 processPointLights(Color color, vec3 normal, vec3 viewDirection, vec3 fragWorldPos)
{
    vec4 result = vec4(0);

    for (int i = 0; i < numberOfPointLights; i++)
    {
        PointLight light = pointLights[i];

        // Ambient
        float ambient = light.ambient;

        // Diffuse
        vec3 lightDirection = normalize(light.position - fragWorldPos);
        float diffuse = max(dot(normal, lightDirection), 0.0) * light.diffuse;

        // Specular
        vec3 halfwayDirection = normalize(lightDirection + viewDirection);
        float specular = pow(max(dot(normal, halfwayDirection), 0.0), model.shininess) * light.specular;

        // Attenuation
        float distance = length(light.position - fragWorldPos);
        float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

        result += (ambient * color.ambient + diffuse * color.diffuse + specular * color.specular) * light.color * attenuation;
    }

    return result;
}

vec4 processPointLightsPbr(PbrParameters pbrParameters, vec3 N /* Normal */, vec3 V /* View Direction*/, vec3 fragWorldPos)
{
    vec3 Lo = vec3(0.0f);

    for (int i = 0; i < numberOfPointLights; ++i)
    {
        PointLight light = pointLights[i];

        // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
        // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
        vec3 F0 = vec3(0.04);
        F0 = mix(F0, pbrParameters.baseColor, pbrParameters.metallic);

        // calculate per-light radiance
        vec3 L = normalize(light.position - fragWorldPos);
        vec3 H = normalize(V + L);

        // attenuation
        float dist = length(light.position - fragWorldPos);
        float attenuation = 1.0f / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

        vec3 radiance = light.color.rgb * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, pbrParameters.roughness);
        float G = GeometrySmith(N, V, L, pbrParameters.roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;

        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - pbrParameters.metallic;

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);

        // add to outgoing radiance Lo
        Lo += (kD * pbrParameters.baseColor / PI + specular) * radiance * NdotL; // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }

    return vec4(Lo, 1.0f);
}

vec4 processHaze(float distance, vec3 fragWorldPos, vec4 subjectColor)
{
    vec4 result = subjectColor;

    if (haze.enabled)
    {
        float factor = exp(-pow(distance * 0.00005f * haze.density, haze.gradient));
        factor = clamp(factor, 0.0f, 1.0f);
        result = mix(vec4(haze.color * clamp(directionalLights[0].direction.y, 0.0f, 1.0f), 1), subjectColor, factor);
    }

    return result;
}

// Reference: https://learnopengl.com/code_viewer_gh.php?code=src/5.advanced_lighting/3.1.3.shadow_mapping/3.1.3.shadow_mapping.fs
float shadowCalculation()
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
    float result = 0.0f;

    for (int x = -samples; x <= samples; ++x)
    {
        for (int y = -samples; y <= samples; ++y)
        {
            float pcfDepth = texture(shadow.map, projCoords.xy + vec2(x, y) * texelSize).r;
            result += currentDepth - shadow.bias > pcfDepth ? 1.0 : 0.0;
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

Color calculateColor(float shadow)
{
    const float lit = 1 - shadow;

    Color color;

    if (model.useColor || !hasAnyColorTexture)
    {
        color.ambient = model.ambient * model.color;
        color.diffuse = model.diffuse * model.color;
        color.specular = model.specular * model.color;
    }
    else
    {
        color.ambient = model.ambient * texture(textureAmbient, fsTextureCoords) * (0.5 + 0.5 * lit);
        color.diffuse = model.diffuse * texture(textureDiffuse, fsTextureCoords) * lit;
        color.specular = model.specular * texture(textureSpecular, fsTextureCoords) * lit;
    }

    if (0.5f < fsVertexColor.a)
    {
        color.ambient = model.ambient * fsVertexColor * (0.5 + 0.5 * lit);
        color.diffuse = model.diffuse * fsVertexColor * lit;
        color.specular = model.specular * fsVertexColor * lit;
    }

    return color;
}

PbrParameters calculatePbrParameters(float shadow)
{
    const float lit = 1 - shadow;

    PbrParameters pbrParameters;

    pbrParameters.baseColor = texture(textureBaseColor, fsTextureCoords).rgb;
    pbrParameters.metallic = texture(textureMetallic, fsTextureCoords).r;
    pbrParameters.roughness = texture(textureRoughness, fsTextureCoords).g;

    if (0.5f < fsVertexColor.a)
    {
        pbrParameters.baseColor = fsVertexColor.rgb * lit;
    }

    return pbrParameters;
}

void main()
{
    vec3 normal = getNormal();

    if (model.invertNormals)
    {
        normal = -normal;
    }

    float shadowFactor = 0.0f;

    if (shadow.enabled)
    {
        shadowFactor = shadowCalculation();
    }

    Color color = calculateColor(shadowFactor);
    PbrParameters pbrParameters = calculatePbrParameters(shadowFactor);

    vec3 viewDirection = normalize(cameraPosition - fsWorldPosition.xyz);
    float distance = length(cameraPosition - fsWorldPosition.xyz);

    // Process
    vec4 result = vec4(0);
    result += processDirectionalLights(color, normal, viewDirection);
    result += processPointLights(color, normal, viewDirection, fsWorldPosition.xyz);
    result += processPointLightsPbr(pbrParameters, normal, viewDirection, fsWorldPosition.xyz);

    // Final
    result = processHaze(distance, fsWorldPosition.xyz, result);

    fragColor = vec4(result.rgb, 1);

    // Fragment position
    fragLocalPosition = vec4(fsLocalPosition.xyz, 1);
    fragWorldPosition = fsWorldPosition;

    // Node Info
    nodeInfo = vec4(nodeId, meshId, float(gl_PrimitiveID), 1);

    // Distance
    outDistance = vec4(distance, 0, 0, 1);

    gl_FragDepth = log2(fsflogz) * (1.0 / log2(zFar + 1.0));
}
