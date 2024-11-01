#version 450 core

struct Model
{
    vec4 color;
    float ambient;
    float diffuse;
    float specular;
    float shininess;
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

uniform Haze haze;
uniform Model model;

uniform PointLight pointLights[8];
uniform int numberOfPointLights;

uniform DirectionalLight directionalLights[8];
uniform int numberOfDirectionalLights;

uniform vec3 cameraPosition;

uniform bool useTextureAmbient;
uniform bool useTextureDiffuse;
uniform bool useTextureSpecular;
uniform bool useTextureNormal;
uniform bool useColor;

uniform sampler2D textureAmbient;
uniform sampler2D textureDiffuse;
uniform sampler2D textureSpecular;
uniform sampler2D textureNormal;

uniform bool invertNormals = false;

// WTF? float?
uniform float nodeID;
uniform float meshID;

in vec4 fsLocalPosition;
in vec4 fsWorldPosition;
in vec3 fsNormal;
in vec2 fsTextureCoords;
in mat3 fsTBN;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 brightColor;
layout(location = 2) out vec4 fragLocalPosition;
layout(location = 3) out vec4 fragWorldPosition;
layout(location = 4) out vec4 nodeInfo;

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

vec4 processDirectionalLights(vec4 ambientColor, vec4 diffuseColor, vec4 specularColor, vec3 normal, vec3 viewDir)
{
    vec4 result = vec4(0);

    for (int i = 0; i < numberOfDirectionalLights; i++)
    {
        // Ambient
        vec4 ambient = ambientColor * directionalLights[i].ambient;

        // Diffuse
        float diffuseStrength = max(dot(normal, -directionalLights[i].direction), 0.0);
        vec4 diffuse = diffuseStrength * diffuseColor * directionalLights[i].diffuse;

        // Specular
        vec3 reflectDir = reflect(directionalLights[i].direction, normal);
        float specularStrength = max(dot(viewDir, reflectDir), 0.0);
        vec4 specular = pow(specularStrength, model.shininess) * specularColor * directionalLights[i].specular;

        result += (ambient + diffuse + specular) * directionalLights[i].color;
    }

    return result;
}

vec4 processPointLights(vec4 ambientColor, vec4 diffuseColor, vec4 specularColor, vec3 normal, vec3 viewDir, vec3 fragWorldPos)
{
    vec4 result = vec4(0);

    for (int i = 0; i < numberOfPointLights; i++)
    {
        // Ambient
        vec4 ambient = ambientColor * pointLights[i].ambient;

        // Diffuse
        vec3 lightDir = normalize(pointLights[i].position - fragWorldPos);
        vec4 diffuse = diffuseColor * max(dot(normal, lightDir), 0.0) * pointLights[i].diffuse;

        // Specular
        vec3 reflectDir = reflect(-lightDir, normal);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        vec4 specular = specularColor * pow(max(dot(normal, halfwayDir), 0.0), model.shininess) * pointLights[i].specular;

        // Attenuation
        float distance = length(pointLights[i].position - fragWorldPos);
        float attenuation = 1.0f / (pointLights[i].constant + pointLights[i].linear * distance + pointLights[i].quadratic * (distance * distance));

        result += (ambient + diffuse + specular) * pointLights[i].color * attenuation;
    }

    return result;
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

void main()
{
    // Common variables
    vec3 normal = getNormal();

    if (invertNormals)
    {
        normal = -normal;
    }

    vec3 viewDir = normalize(cameraPosition - fsWorldPosition.xyz);
    float distance = length(cameraPosition - fsWorldPosition.xyz);

    vec4 ambientColor = vec4(0);
    vec4 diffuseColor = vec4(0);
    vec4 specularColor = vec4(0);

    if (useColor)
    {
        ambientColor = model.ambient * model.color;
        diffuseColor = model.diffuse * model.color;
        specularColor = model.specular * model.color;
    }
    else
    {
        if (useTextureAmbient)
        {
            ambientColor = model.ambient * texture(textureAmbient, fsTextureCoords);
        }

        if (useTextureDiffuse)
        {
            diffuseColor = model.diffuse * texture(textureDiffuse, fsTextureCoords);
        }

        if (useTextureSpecular)
        {
            specularColor = model.specular * texture(textureSpecular, fsTextureCoords);
        }
    }

    // Process
    vec4 result = vec4(0);
    result += processDirectionalLights(ambientColor, diffuseColor, specularColor, normal, viewDir);
    result += processPointLights(ambientColor, diffuseColor, specularColor, normal, viewDir, fsWorldPosition.xyz);

    // Final
    result = processHaze(distance, fsWorldPosition.xyz, result);
    // result = mix(result, model.overlayColor, model.overlayColorFactor);
    // result = mix(result, model.meshOverlayColor, model.meshOverlayColorFactor);

    fragColor = vec4(result.rgb, 1);

    float brightness = dot(result.rgb, vec3(0.2126f, 0.7152f, 0.0722f));

    if (brightness > 1.0f)
        brightColor = vec4(result.rgb, 1.0f);
    else
        brightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);

    // Fragment position
    fragLocalPosition = vec4(fsLocalPosition.xyz, 1.0f);
    fragWorldPosition = fsWorldPosition;

    // Node Info
    nodeInfo = vec4(nodeID, meshID, 0, 1);
}
