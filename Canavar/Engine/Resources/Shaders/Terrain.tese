// Taken from https://github.com/AntonHakansson/procedural-terrain
#version 450

layout(triangles, equal_spacing, ccw) in;

in DATA
{
    vec3 world_pos;
    vec2 tex_coord;
    vec3 normal;
}
In[];

uniform mat4 VP;
uniform mat4 V;
uniform float z_far;

// Noise
struct Noise
{
    int num_octaves;
    float amplitude;
    float frequency;
    float persistence;
    float lacunarity;
};

uniform Noise noise;

// Out data
out DATA
{
    vec3 world_pos;
    vec3 view_space_pos;
    vec3 view_space_normal;
    vec2 tex_coord;
    vec3 normal;
    vec3 tangent;
    vec3 bitangent;
    mat3 tangent_matrix;
    float f_log_z;
}
Out;

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

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
    return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

float terrain_height(vec2 pos)
{
    float noise_value = 0;
    float frequency = noise.frequency;
    float amplitude = noise.amplitude;

    for (int i = 0; i < noise.num_octaves; i++)
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
            // n = voronoi2d(pos * frequency / 400.0 + vec2(112, 532));
            n = snoise(pos * frequency / 800.0 + vec2(1231, 721)) / 2;
        }

        noise_value += n * amplitude;
        amplitude *= noise.persistence;
        frequency *= noise.lacunarity;
    }

    return noise_value; // + snoise(pos / 10000.0) * 500.0 +  + max(0, voronoi2d(pos / 5000.0) *
                        // 1000.0);
}

vec3 computeNormal(vec3 WorldPos)
{
    vec2 eps = vec2(0.1, 0.0);
    return normalize(vec3(terrain_height(WorldPos.xz - eps.xy) - terrain_height(WorldPos.xz + eps.xy), //
                          2 * eps.x,
                          terrain_height(WorldPos.xz - eps.yx) - terrain_height(WorldPos.xz + eps.yx)));
}

void main()
{
    // Interpolate the attributes of the output vertex using the barycentric coordinates
    Out.world_pos = interpolate3D(In[0].world_pos, In[1].world_pos, In[2].world_pos);
    Out.tex_coord = interpolate2D(In[0].tex_coord, In[1].tex_coord, In[2].tex_coord);

    // Displace the vertex along the normal
    float displacement = terrain_height(Out.world_pos.xz);
    Out.world_pos += vec3(0.0, 1.0, 0.0) * displacement;
    Out.view_space_pos = (V * vec4(Out.world_pos, 1.0)).xyz;
    Out.normal = computeNormal(Out.world_pos);
    Out.view_space_normal = (V * vec4(Out.normal, 0.0)).xyz;
    Out.tangent = normalize(cross(Out.normal, vec3(0, 1, 0)));
    Out.bitangent = normalize(cross(Out.tangent, Out.normal));
    Out.tangent_matrix = mat3(Out.tangent, Out.bitangent, Out.normal);

    gl_Position = VP * vec4(Out.world_pos, 1.0);

    float coef = 2.0 / log2(z_far + 1.0);
    gl_Position.z = log2(max(1e-6, 1.0 + gl_Position.w)) * coef - 1.0;
    Out.f_log_z = 1.0 + gl_Position.w;
}
