#version 330 core
// This code is taken from https://github.com/fede-vaccaro/TerrainEngine-OpenGL and adopted.
// MIT License

// Copyright (c) 2018 Federico Vaccaro

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

in vec3 fsWorldPosition;
in vec3 fsNormal;
in vec2 fsTextureCoord;
in float fsDistanceFromPosition;
in float fsHeight;

struct Sun
{
    vec4 color;
    vec3 direction;
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

struct Terrain
{
    vec3 seed;
    int octaves;
    float tessellationMultiplier;
    float amplitude;
    float frequency;
    float power;
    float grassCoverage;
    float ambient;
    float diffuse;
    float shininess;
    float specular;
};

struct Haze
{
    bool enabled;
    vec3 color;
    float density;
    float gradient;
};

uniform Sun sun;
uniform Terrain terrain;
uniform Haze haze;
uniform PointLight pointLights[8];

uniform int numberOfPointLights;
uniform vec3 cameraPos;
uniform float waterHeight;
uniform sampler2D sand, grass, terrainTexture, snow, rock, rockNormal;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

const mat2 m = mat2(0.8, -0.6, 0.6, 0.8);

float noise(int ind, int x, int y)
{
    const int primes[39] = int[39](995615039,
                                   600173719,
                                   701464987,
                                   831731269,
                                   162318869,
                                   136250887,
                                   174329291,
                                   946737083,
                                   245679977,
                                   362489573,
                                   795918041,
                                   350777237,
                                   457025711,
                                   880830799,
                                   909678923,
                                   787070341,
                                   177340217,
                                   593320781,
                                   405493717,
                                   291031019,
                                   391950901,
                                   458904767,
                                   676625681,
                                   424452397,
                                   531736441,
                                   939683957,
                                   810651871,
                                   997169939,
                                   842027887,
                                   423882827,
                                   192405871,
                                   129403928,
                                   102948294,
                                   102948203,
                                   912030912,
                                   910204948,
                                   958730910,
                                   759204855,
                                   859302983);

    int n = x + y * 57;
    n = (n << 13) ^ n;
    int a = primes[ind * 3], b = primes[ind * 3 + 1], c = primes[ind * 3 + 2];
    int t = (n * (n * n * a + b) + c) & 0x7fffffff;
    return float(1.0 - t / 1073741824.0);
}

// Dummy random
float random2d(vec2 st)
{
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233) + terrain.seed.xy)) * 43758.5453123);
}

// Cosine interpolation
float interpolate(float a, float b, float x)
{
    float ft = x * 3.14159265f;
    float f = (1 - cos(ft)) * 0.5;
    return a * (1 - f) + b * f;
}

float interpolatedNoise(vec2 xy)
{
    float x = xy.x, y = xy.y;
    int integer_X = int(floor(x));
    float fractional_X = fract(x);
    int integer_Y = int(floor(y));
    float fractional_Y = fract(y);
    vec2 randomInput = vec2(integer_X, integer_Y);
    float a = random2d(randomInput);
    float b = random2d(randomInput + vec2(1.0, 0.0));
    float c = random2d(randomInput + vec2(0.0, 1.0));
    float d = random2d(randomInput + vec2(1.0, 1.0));

    vec2 w = vec2(fractional_X, fractional_Y);
    w = w * w * w * (10.0 + w * (-15.0 + 6.0 * w));

    float k0 = a, k1 = b - a, k2 = c - a, k3 = d - c - b + a;

    return k0 + k1 * w.x + k2 * w.y + k3 * w.x * w.y;
}

float smoothstepd(float x)
{
    return 6.0 * x * (1.0 - x);
}

vec2 smoothstepd(float a, float b, float x)
{
    if (x < a)
        return vec2(0.0, 0.0);
    if (x > b)
        return vec2(1.0, 0.0);
    float ir = 1.0 / (b - a);
    x = (x - a) * ir;
    return vec2(x * x * (3.0 - 2.0 * x), 6.0 * x * (1.0 - x) * ir);
}

vec2 interpolatedNoiseD(vec2 xy)
{
    float x = xy.x, y = xy.y;
    int integer_X = int(floor(x));
    float fractional_X = fract(x);
    int integer_Y = int(floor(y));
    float fractional_Y = fract(y);

    vec2 randomInput = vec2(integer_X, integer_Y);
    float a = random2d(randomInput + vec2(0.0, 0.0));
    float b = random2d(randomInput + vec2(1.0, 0.0));
    float c = random2d(randomInput + vec2(0.0, 1.0));
    float d = random2d(randomInput + vec2(1.0, 1.0));

    float k0 = a, k1 = b - a, k2 = c - a, k3 = d - c - b + a;

    float dndx = (k1 + k3 * fractional_Y) * smoothstepd(fractional_X);
    float dndy = (k2 + k3 * fractional_X) * smoothstepd(fractional_Y);

    return vec2(dndx, dndy);
}

float perlin(float x, float y)
{
    vec2 st = vec2(x, y);
    float persistence = 0.5;
    float total = 0;
    float frequency = 0.005 * terrain.frequency;
    float amplitude = terrain.amplitude;
    for (int i = 0; i < terrain.octaves; ++i)
    {
        frequency *= 2.0;
        amplitude *= persistence;
        vec2 v = frequency * m * st;
        total += interpolatedNoise(v) * amplitude;
    }
    return pow(total, terrain.power);
}

vec3 computeNormals(vec3 worldPos, out mat3 tbn)
{
    float st = 1.0;
    float dhdu = (perlin((worldPos.x + st), worldPos.z) - perlin((worldPos.x - st), worldPos.z)) / (2.0 * st);
    float dhdv = (perlin(worldPos.x, (worldPos.z + st)) - perlin(worldPos.x, (worldPos.z - st))) / (2.0 * st);

    vec3 X = vec3(1.0, dhdu, 1.0);
    vec3 Z = vec3(0.0, dhdv, 1.0);

    vec3 n = normalize(cross(Z, X));
    tbn = mat3(normalize(X), normalize(Z), n);

    return n;
}

vec3 computeNormals(vec2 gradient)
{
    vec3 X = vec3(1.0, gradient.r, 0.0);
    vec3 Z = vec3(0.0, gradient.g, 1.0);

    vec3 n = normalize(cross(Z, X));

    return n;
}

float perlin(float x, float y, int oct)
{
    vec2 distances = vec2(500.0, 2000.0);
    int distanceFactor = int(clamp((distances.y - fsDistanceFromPosition) * 3.0 / (distances.y - distances.x), 0.0, 3.0));
    distanceFactor = 3 - distanceFactor;

    float persistence = 0.5;
    float total = 0;
    float frequency = 0.05 * terrain.frequency;
    float ampl = 1.0;
    for (int i = 0; i < terrain.octaves; ++i)
    {
        frequency *= 2;
        ampl *= persistence;
        total += interpolatedNoise(vec2(x, y) * frequency) * ampl;
    }
    return total;
}

vec4 getTexture(inout vec3 normal, const mat3 TBN)
{
    float trans = 20.;

    vec4 sand_t = texture(sand, fsTextureCoord * 10.0);
    sand_t.rg *= 1.3;
    vec4 rock_t = texture(rock, fsTextureCoord * vec2(1.0, 1.256).yx);
    rock_t.rgb *= vec3(2.5, 2.0, 2.0);
    vec4 grass_t = texture(grass, fsTextureCoord * 10.0);
    vec4 grass_t1 = texture(terrainTexture, fsTextureCoord * 10.0);
    float perlinBlendingCoeff = clamp(perlin(fsWorldPosition.x, fsWorldPosition.z, 2) * 2.0 - 0.2, 0.0, 1.0);
    grass_t = mix(grass_t * 1.3, grass_t1 * 0.75, perlinBlendingCoeff);
    grass_t.rgb *= 0.5;

    vec4 heightColor;
    float cosV = abs(dot(normal, vec3(0.0, 1.0, 0.0)));
    float tenPercentGrass = terrain.grassCoverage - terrain.grassCoverage * 0.1;
    float blendingCoeff = pow((cosV - tenPercentGrass) / (terrain.grassCoverage * 0.1), 1.0);

    if (fsHeight <= waterHeight + trans)
    {
        heightColor = sand_t;
    } else if (fsHeight <= waterHeight + 2 * trans)
    {
        heightColor = mix(sand_t, grass_t, pow((fsHeight - waterHeight - trans) / trans, 1.0));
    } else if (cosV > terrain.grassCoverage)
    {
        heightColor = grass_t;
        mix(normal, vec3(0.0, 1.0, 0.0), 0.25);
    } else if (cosV > tenPercentGrass)
    {
        heightColor = mix(rock_t, grass_t, blendingCoeff);
        normal = mix(TBN * (texture(rockNormal, fsTextureCoord * vec2(2.0, 2.5).yx).rgb * 2.0 - 1.0), normal, blendingCoeff);
    } else
    {
        heightColor = rock_t;
        normal = TBN * (texture(rockNormal, fsTextureCoord * vec2(2.0, 2.5).yx).rgb * 2.0 - 1.0);
    }

    return heightColor;
}

vec4 processSun(vec4 color, vec3 normal, vec3 viewDir)
{
    // Ambient
    vec4 ambient = color * terrain.ambient * sun.ambient;

    // Diffuse
    vec4 diffuse = color * max(dot(normal, sun.direction), 0.0) * sun.diffuse * terrain.diffuse;

    // Specular
    vec3 reflectDir = reflect(-sun.direction, normal);
    vec3 halfwayDir = normalize(sun.direction + viewDir);
    vec4 specular = color * pow(max(dot(normal, halfwayDir), 0.0), terrain.shininess) * terrain.specular * sun.specular;

    return (ambient + diffuse + specular) * sun.color;
}


vec4 processPointLights(vec4 color, vec3 normal, vec3 viewDir, vec3 fragWorldPos)
{
    vec4 result = vec4(0);

    for (int i = 0; i < numberOfPointLights; i++)
    {
        // Ambient
        vec4 ambient = color * pointLights[i].ambient * pointLights[i].ambient;

        // Diffuse
        vec3 lightDir = normalize(pointLights[i].position - fragWorldPos);
        vec4 diffuse =  color * max(dot(normal, lightDir), 0.0) * pointLights[i].diffuse * terrain.diffuse;

        // Specular
        vec3 reflectDir = reflect(-lightDir, normal);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        vec4 specular = color * pow(max(dot(normal, halfwayDir), 0.0), terrain.shininess) * pointLights[i].specular * terrain.specular;

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
    vec3 viewDir = normalize(cameraPos - fsWorldPosition);
    float distance = length(cameraPos - fsWorldPosition);

    mat3 TBN;
    vec3 normal = computeNormals(fsWorldPosition, TBN);
    normal = normalize(normal);

    vec4 heightColor = getTexture(normal, TBN);

    vec4 result = vec4(0);
    result += processSun(heightColor, normal, viewDir);
    result += processPointLights(heightColor, normal, viewDir, fsWorldPosition);

    // Final
    fragColor = processHaze(distance, fsWorldPosition, vec4(result.rgb, 1));

    float brightness = dot(fragColor.rgb, vec3(0.2126f, 0.7152f, 0.0722f));
    if(brightness > 1.0f)
        brightColor = vec4(fragColor.rgb, 1.0f);
    else
        brightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
};
