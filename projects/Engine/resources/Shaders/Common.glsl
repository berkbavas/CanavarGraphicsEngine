struct Model
{
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
        result =  mix(vec4(haze.color, 1) * clamp(sun.direction.y, 0.0f, 1.0f), subjectColor, factor);
    }

    return result;
}
