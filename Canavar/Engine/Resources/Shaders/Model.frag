#version 430 core

struct Model
{
    vec3 color;
    float ambient;
    float diffuse;
    float specular;
    float shininess;
    bool useModelColor;
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
uniform sampler2D textureNormal;

uniform bool hasTextureBaseColor;
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

void main()
{
    vec3 normal = CalculateNormal();
    vec3 color = CalculateColor();
    vec3 Lo = normalize(cameraPosition - fsWorldPosition);
    float distance = length(cameraPosition - fsWorldPosition);

    // Process
    vec3 result = vec3(0.0f);
    result += ProcessDirectionalLights(color, normal, Lo);
    result += ProcessPointLights(color, normal, Lo, fsWorldPosition);

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
