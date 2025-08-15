#version 450 core

// Taken from https://github.com/AntonHakansson/procedural-terrain

layout(triangles, equal_spacing, ccw) in;

uniform mat4 uViewProjectionMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uLightViewProjectionMatrix; // Light view-projection matrix
uniform float uZFar;

// Noise
struct Noise
{
    int octaves;
    float amplitude;
    float frequency;
    float persistence;
    float lacunarity;
};

uniform Noise uNoise;

in vec3 teWorldPosition[];
in vec2 teTextureCoords[];
in vec3 teNormal[];

out vec3 fsWorldPosition;
out vec3 fsViewSpacePosition;
out vec3 fsViewSpaceNormal;
out vec2 fsTextureCoords;
out vec3 fsNormal;
out vec3 fsTangent;
out vec3 fsBitangent;
out mat3 fsTangentMatrix;
out float fsLogZ;
out vec4 fsLightSpacePosition;

vec3 mod289(vec3 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec2 mod289(vec2 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec3 permute(vec3 x)
{
    return mod289(((x * 34.0) + 1.0) * x);
}

float snoise(vec2 v)
{
    vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                  0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                  -0.577350269189626, // -1.0 + 2.0 * C.x
                  0.024390243902439); // 1.0 / 41.0
    // First corner
    vec2 i = floor(v + dot(v, C.yy));
    vec2 x0 = v - i + dot(i, C.xx);

    // Other corners
    vec2 i1;
    // i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
    // i1.y = 1.0 - i1.x;
    i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    // x0 = x0 - 0.0 + 0.0 * C.xx ;
    // x1 = x0 - i1 + 1.0 * C.xx ;
    // x2 = x0 - 1.0 + 2.0 * C.xx ;
    vec4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;

    // Permutations
    i = mod289(i); // Avoid truncation effects in permutation
    vec3 p = permute(permute(i.y + vec3(0.0, i1.y, 1.0)) + i.x + vec3(0.0, i1.x, 1.0));

    vec3 m = max(0.5 - vec3(dot(x0, x0), dot(x12.xy, x12.xy), dot(x12.zw, x12.zw)), 0.0);
    m = m * m;
    m = m * m;

    // Gradients: 41 points uniformly over a line, mapped onto a diamond.
    // The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

    vec3 x = 2.0 * fract(p * C.www) - 1.0;
    vec3 h = abs(x) - 0.5;
    vec3 ox = floor(x + 0.5);
    vec3 a0 = x - ox;

    // Normalise gradients implicitly by scaling m
    // Approximation of: m *= inversesqrt( a0*a0 + h*h );
    m *= 1.79284291400159 - 0.85373472095314 * (a0 * a0 + h * h);

    // Compute final noise value at P
    vec3 g;
    g.x = a0.x * x0.x + h.x * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}

const mat2 myt = mat2(.12121212, .13131313, -.13131313, .12121212);
const vec2 mys = vec2(1e4, 1e6);

vec2 rhash(vec2 uv)
{
    uv *= myt;
    uv *= mys;
    return fract(fract(uv / mys) * uv);
}

vec3 hash(vec3 p)
{
    return fract(sin(vec3(dot(p, vec3(1.0, 57.0, 113.0)), dot(p, vec3(57.0, 113.0, 1.0)), dot(p, vec3(113.0, 1.0, 57.0)))) * 43758.5453);
}

float voronoi2d(const in vec2 point)
{
    vec2 p = floor(point);
    vec2 f = fract(point);
    float res = 0.0;
    for (int j = -1; j <= 1; j++)
    {
        for (int i = -1; i <= 1; i++)
        {
            vec2 b = vec2(i, j);
            vec2 r = vec2(b) - f + rhash(p + b);
            res += 1. / pow(dot(r, r), 8.);
        }
    }
    return pow(1. / res, 0.0625);
}

vec2 Interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
    return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 Interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

float TerrainHeight(vec2 pos)
{
    float noiseValue = 0;
    float frequency = uNoise.frequency;
    float amplitude = uNoise.amplitude;

    for (int i = 0; i < uNoise.octaves; i++)
    {
        float n = 0;

        if (i == 0)
        {
            n = pow(voronoi2d(pos * frequency / 200.0), 2);
        }
        else if (i == 1)
        {
            n = snoise(pos * frequency / 400.0) / 1.5;
        }
        else
        {
            n = snoise(pos * frequency / 800.0 + vec2(1231, 721)) / 2;
        }

        noiseValue += n * amplitude;
        amplitude *= uNoise.persistence;
        frequency *= uNoise.lacunarity;
    }

    return noiseValue;
}

vec3 ComputeNormal(vec3 worldPos)
{
    vec2 eps = vec2(0.1, 0.0);
    return normalize(vec3(TerrainHeight(worldPos.xz - eps.xy) - TerrainHeight(worldPos.xz + eps.xy), //
                          2 * eps.x,
                          TerrainHeight(worldPos.xz - eps.yx) - TerrainHeight(worldPos.xz + eps.yx)));
}

void main()
{
    // Interpolate the attributes of the output vertex using the barycentric coordinates
    fsWorldPosition = Interpolate3D(teWorldPosition[0], teWorldPosition[1], teWorldPosition[2]);
    fsTextureCoords = Interpolate2D(teTextureCoords[0], teTextureCoords[1], teTextureCoords[2]);

    // Displace the vertex along the normal
    float displacement = TerrainHeight(fsWorldPosition.xz);
    fsWorldPosition += vec3(0.0f, 1.0f, 0.0f) * displacement;
    fsViewSpacePosition = (uViewMatrix * vec4(fsWorldPosition, 1.0f)).xyz;
    fsNormal = ComputeNormal(fsWorldPosition);
    fsTangent = normalize(cross(fsNormal, vec3(0.0f, 1.0f, 0.0f)));
    fsBitangent = normalize(cross(fsTangent, fsNormal));
    fsTangentMatrix = mat3(fsTangent, fsBitangent, fsNormal);
    fsLightSpacePosition = uLightViewProjectionMatrix * vec4(fsWorldPosition, 1.0f);

    gl_Position = uViewProjectionMatrix * vec4(fsWorldPosition, 1.0f);

    float coef = 2.0f / log2(uZFar + 1.0f);
    gl_Position.z = log2(max(1e-6, 1.0f + gl_Position.w)) * coef - 1.0f;
    fsLogZ = 1.0f + gl_Position.w;
}
