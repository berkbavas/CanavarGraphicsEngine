#version 330 core
out vec4 outputColor;

struct Node {
    mat4 transformation;
    vec4 color;
    float ambient;
    float diffuse;
    float specular;
    float shininess;
};

struct Light {
    vec4 color;
    vec3 position;
    float ambient;
    float diffuse;
    float specular;
};

in vec3 fPosition;
in vec3 fNormal;

uniform vec3 cameraPosition;
uniform Node node;
uniform Light light;

void main()
{

    // ambient
    float ambient = light.ambient * node.ambient;

    // diffuse
    vec3 norm = normalize(fNormal);
    vec3 lightDir = normalize(light.position - fPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    float diffuse = light.diffuse * (diff * node.diffuse);

    // specular
    vec3 viewDir = normalize(cameraPosition - fPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), node.shininess);
    float specular = light.specular * (spec * node.specular);


    outputColor = (specular + ambient + diffuse) * node.color * light.color;
}
