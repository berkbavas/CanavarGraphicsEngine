#version 330 core

struct Model
{
    vec4 overlayColor;
    vec4 meshOverlayColor;
    float overlayColorFactor;
    float meshOverlayColorFactor;
    float ambient;
    float diffuse;
    float specular;
    float shininess;
};

struct Sun
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

uniform Haze haze;
uniform Sun sun;
uniform Model model;

uniform PointLight pointLights[8];
uniform int numberOfPointLights;

uniform vec3 cameraPos;

uniform bool useTextureAmbient;
uniform bool useTextureDiffuse;
uniform bool useTextureSpecular;
uniform bool useTextureNormal;

uniform sampler2D textureAmbient;
uniform sampler2D textureDiffuse;
uniform sampler2D textureSpecular;
uniform sampler2D textureNormal;

in vec4 fsPosition;
in vec3 fsNormal;
in vec2 fsTextureCoords;
in mat3 fsTBN;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

vec3 getNormal()
{
    if (useTextureNormal)
    {
        vec3 normal = texture(textureNormal, fsTextureCoords).rgb;
        normal = 2.0 * normal - 1.0;
        normal = normalize(fsTBN * normal);
        return normal;
    } else
        return fsNormal;
}


vec4 processSun(vec4 ambientColor, vec4 diffuseColor, vec4 specularColor, vec3 normal, vec3 viewDir)
{
    // Ambient
    vec4 ambient = ambientColor * model.ambient * sun.ambient;

    // Diffuse
    vec4 diffuse = diffuseColor * max(dot(normal, sun.direction), 0.0) * sun.diffuse * model.diffuse;

    // Specular
    vec3 reflectDir = reflect(-sun.direction, normal);
    vec3 halfwayDir = normalize(sun.direction + viewDir);
    vec4 specular = specularColor * pow(max(dot(normal, halfwayDir), 0.0), model.shininess) * model.specular * sun.specular;

    return (ambient + diffuse + specular) * sun.color;
}

vec4 processPointLights(vec4 ambientColor, vec4 diffuseColor, vec4 specularColor, vec3 normal, vec3 viewDir, vec3 fragWorldPos)
{
    vec4 result = vec4(0);

    for (int i = 0; i < numberOfPointLights; i++)
    {
        // Ambient
        vec4 ambient = ambientColor * pointLights[i].ambient * pointLights[i].ambient;

        // Diffuse
        vec3 lightDir = normalize(pointLights[i].position - fragWorldPos);
        vec4 diffuse =  diffuseColor * max(dot(normal, lightDir), 0.0) * pointLights[i].diffuse * model.diffuse;

        // Specular
        vec3 reflectDir = reflect(-lightDir, normal);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        vec4 specular = specularColor * pow(max(dot(normal, halfwayDir), 0.0), model.shininess) * pointLights[i].specular * model.specular;

        // Attenuation
        float distance = length(pointLights[i].position - fragWorldPos);
        float attenuation = 1.0f / (pointLights[i].constant + pointLights[i].linear * distance + pointLights[i].quadratic * (distance * distance));

        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;

        result += (ambient + diffuse + specular) * pointLights[i].color;
    }

    return result;
}

vec4 processHaze(float distance, vec3 fragWorldPos, vec4 subjectColor)
{
    vec4 result = subjectColor;

    if(haze.enabled)
    {
        float factor = exp(-pow(distance * 0.00005f * haze.density, haze.gradient));
        factor = clamp(factor, 0.0f, 1.0f);
        result =  mix(vec4(haze.color * clamp(sun.direction.y, 0.0f, 1.0f), 1) , subjectColor, factor);
    }

    return result;
}

void main()
{
    // Common variables
    vec3 normal = getNormal();
    vec3 viewDir = normalize(cameraPos - fsPosition.xyz);
    float distance = length(cameraPos - fsPosition.xyz);

    vec4 ambientColor = vec4(0);
    vec4 diffuseColor = vec4(0);
    vec4 specularColor = vec4(0);

    if (useTextureAmbient)
        ambientColor = texture(textureAmbient, fsTextureCoords);

    if (useTextureDiffuse)
        diffuseColor = texture(textureDiffuse, fsTextureCoords);

    if (useTextureSpecular)
        specularColor = texture(textureSpecular, fsTextureCoords);

    // Process
    vec4 result = vec4(0);
    result += processSun(ambientColor, diffuseColor, specularColor, normal, viewDir);
    result += processPointLights(ambientColor, diffuseColor, specularColor, normal, viewDir, fsPosition.xyz);

    // Final
    result = processHaze(distance, fsPosition.xyz, result);
    result = mix(result, model.overlayColor, model.overlayColorFactor);
    result = mix(result, model.meshOverlayColor, model.meshOverlayColorFactor);

    fragColor = vec4(result.xyz, 1);

    float brightness = dot(result.rgb, vec3(0.2126f, 0.7152f, 0.0722f));
    if(brightness > 1.0f)
        brightColor = vec4(result.rgb, 1.0f);
    else
        brightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
