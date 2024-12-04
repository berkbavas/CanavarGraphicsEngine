#version 430 core

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

struct Color
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

uniform Haze haze;
uniform Model model;

uniform PointLight pointLights[8];
uniform int numberOfPointLights;

uniform DirectionalLight directionalLights[8];
uniform int numberOfDirectionalLights;

uniform vec3 cameraPosition;
uniform vec3 sunDirection;

uniform bool useTextureAmbient;
uniform bool useTextureDiffuse;
uniform bool useTextureSpecular;
uniform bool useTextureNormal;
uniform bool useColor;

uniform sampler2D textureAmbient;
uniform sampler2D textureDiffuse;
uniform sampler2D textureSpecular;
uniform sampler2D textureNormal;

uniform sampler2D shadowMap;
uniform bool shadowsEnabled;
uniform float shadowBiasCoefficent;

uniform bool invertNormals = false;

// WTF? float?
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
float shadowCalculation(vec3 normal)
{
    // perform perspective divide
    vec3 projCoords = fsFragPosLightSpace.xyz / fsFragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    float bias = 0.00001f * max((1.0f - dot(normal, sunDirection)), 0.1f);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -3; x <= 3; ++x)
    {
        for (int y = -3; y <= 3; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - shadowBiasCoefficent * bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 49.0;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if (projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

Color calculateColor(float shadow)
{
    const float lit = 1 - shadow;

    Color color;

    if (useColor)
    {
        color.ambient = model.ambient * model.color * (0.5 + 0.5 * lit);
        color.diffuse = model.diffuse * model.color * lit;
        color.specular = model.specular * model.color * lit;
    }
    else
    {
        if (useTextureAmbient)
        {
            color.ambient = model.ambient * texture(textureAmbient, fsTextureCoords) * (0.5 + 0.5 * lit);
        }

        if (useTextureDiffuse)
        {
            color.diffuse = model.diffuse * texture(textureDiffuse, fsTextureCoords) * lit;
        }

        if (useTextureSpecular)
        {
            color.specular = model.specular * texture(textureSpecular, fsTextureCoords) * lit;
        }
    }

    if (0.9f < fsVertexColor.a)
    {
        color.ambient = model.ambient * fsVertexColor * (0.5 + 0.5 * lit);
        color.diffuse = model.diffuse * fsVertexColor * lit;
        color.specular = model.specular * fsVertexColor * lit;
    }
    
    return color;
}

void main()
{
    vec3 normal = getNormal();

    if (invertNormals)
    {
        normal = -normal;
    }

    float shadow = 0.0f;

    if (shadowsEnabled)
    {
        shadow = shadowCalculation(normal);
    }

    Color color = calculateColor(shadow);

    vec3 viewDirection = normalize(cameraPosition - fsWorldPosition.xyz);
    float distance = length(cameraPosition - fsWorldPosition.xyz);

    // Process
    vec4 result = vec4(0);
    result += processDirectionalLights(color, normal, viewDirection);
    result += processPointLights(color, normal, viewDirection, fsWorldPosition.xyz);

    // Final
    result = processHaze(distance, fsWorldPosition.xyz, result);

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
    nodeInfo = vec4(nodeId, meshId, float(gl_PrimitiveID), 1);
}
