// Taken from https://github.com/AntonHakansson/procedural-terrain
#version 450

#define PI 3.14159265359

in DATA
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
In;

struct Terrain
{
    float ambient;
    float diffuse;
    float specular;
    float shininess;
};

struct Haze
{
    bool enabled;
    vec3 color;
    float density;
    float gradient;
};

struct Noise
{
    int num_octaves;
    float amplitude;
    float frequency;
    float persistence;
    float lacunarity;
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

layout(binding = 0) uniform sampler2DArray albedos;
layout(binding = 1) uniform sampler2DArray normals;
layout(binding = 2) uniform sampler2DArray displacement;

/**
 * Output
 */
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 FragLocalPosition;
layout(location = 2) out vec4 FragWorldPosition;
layout(location = 3) out vec4 FragNodeInfo;

uniform vec3 eye_world_pos;
uniform float z_far;

uniform float texture_start_heights[4];
uniform float texture_blends[4];
uniform float texture_sizes[4];
uniform float texture_displacement_weights[4];

uniform Noise noise;
uniform Haze haze;
uniform Terrain terrain;

uniform PointLight point_lights[8];
uniform int number_of_point_lights;

uniform DirectionalLight directional_lights[8]; // First element is the Sun
uniform int number_of_directional_lights;

uniform float node_id; // QOpenGLFramebuffer messes up when the internal texture format is GL_RGBA32UI.

/**
 * Functions
 */

float inverseLerp(float a, float b, float x)
{
    return (x - a) / (b - a);
}
float inverseLerpClamped(float a, float b, float x)
{
    return clamp(inverseLerp(a, b, x), 0, 1);
}

vec3 getWorldNormal(vec3 tex_coord)
{
    vec3 tangent_normal = normalize(2.0 * texture(normals, tex_coord).xyz - 1.0);
    return In.tangent_matrix * tangent_normal;
}

float[4] getTextureWeightsByDisplacement(vec3[4] t, float[4] a)
{
    const float depth = 0.1;

    float disp[t.length];
    for (int i = 0; i < t.length; i++)
    {
        disp[i] = texture(displacement, t[i]).r * texture_displacement_weights[i];
    }

    float ma = 0;
    for (int i = 0; i < t.length; i++)
    {
        ma = max(ma, disp[i] + a[i]);
    }
    ma -= depth;

    float total_w = 0;
    float[t.length] w;
    for (int i = 0; i < t.length; i++)
    {
        w[i] = max(disp[i] + a[i] - ma, 0);
        total_w += w[i];
    }

    // normalize
    for (int i = 0; i < t.length; i++)
    {
        w[i] /= total_w;
    }

    return w;
}

float[4] terrainBlending(vec3 world_pos, vec3 normal)
{
    float height = world_pos.y;

    // A completely flat terrain has slope=0
    float slope = max(1 - dot(normal, vec3(0, 1, 0)), 0.0);

    // For each fragment we compute how much each texture contributes depending on height and slope
    float draw_strengths[4];

    float sand_grass_height = texture_start_heights[0];
    float grass_rock_height = texture_start_heights[1];
    float rock_snow_height = texture_start_heights[2];

    float grass_falloff = texture_blends[1];
    float rock_falloff = texture_blends[2];
    float snow_falloff = texture_blends[3];

    float a, b, c, d;
    {
        b = inverseLerpClamped(sand_grass_height - grass_falloff / 2, sand_grass_height + grass_falloff / 2, height);
    }
    {
        c = inverseLerpClamped(grass_rock_height - rock_falloff / 2, grass_rock_height + rock_falloff / 2, height);
    }

    float b_in, c_in, d_in;
    b_in = inverseLerpClamped(sand_grass_height - grass_falloff / 2, sand_grass_height + grass_falloff / 2, height);
    c_in = inverseLerpClamped(grass_rock_height - rock_falloff / 2, grass_rock_height + rock_falloff / 2, height);
    d_in = inverseLerpClamped(rock_snow_height - snow_falloff / 2, rock_snow_height + snow_falloff / 2, height);

    a = 1 - b_in;
    b *= 1 - c_in;
    c *= 1 - d_in;
    d = d_in;

    // a *= 1 - inverseLerpClamped(0.1, 1, slope);
    b *= 1 - inverseLerpClamped(0.1, 1.0, slope);
    d *= 1 - inverseLerpClamped(0.1, 1, slope);

    float tot = a + b + c + d;
    draw_strengths[0] = max(a / tot, 0);
    draw_strengths[1] = max(b / tot, 0);
    draw_strengths[2] = max(c / tot, 0);
    draw_strengths[3] = max(d / tot, 0);

    return draw_strengths;
}

vec3 getTextureCoordinate(vec3 world_pos, int texture_index)
{
    return vec3(In.tex_coord * texture_sizes[texture_index], texture_index);
}

vec3 terrainNormal(vec3 world_pos, vec3 normal)
{
    float[] draw_strengths = terrainBlending(world_pos, normal);

    vec3 norm = vec3(0);

    for (int i = 0; i < 4; i++)
    {
        float d = draw_strengths[i];
        vec3 normal = normalize(2.0 * texture(normals, getTextureCoordinate(world_pos, i)).xyz - 1.0);
        norm = norm * (1 - draw_strengths[i]) + (In.tangent_matrix * normal) * d;
    }

    return norm;
}

vec3 process_directional_lights(vec3 color, vec3 normal, vec3 light_out)
{
    vec3 result = vec3(0.0f);

    for (int i = 0; i < number_of_directional_lights; i++)
    {
        DirectionalLight light = directional_lights[i];

        // Ambient
        float ambient = terrain.ambient * light.ambient;

        // Diffuse
        vec3 ld = -light.direction;
        float df = dot(normal, ld);
        float diffuse = max(df, 0.0f) * terrain.diffuse * light.diffuse;

        // Specular
        vec3 hd = normalize(ld + light_out);
        float sf = max(dot(normal, hd), 0.0);
        float specular = pow(sf, terrain.shininess) * terrain.specular * light.specular;

        result += (ambient + diffuse + specular) * color * light.color;
    }

    return result;
}

vec3 process_point_lights(vec3 color, vec3 normal, vec3 light_out, vec3 world_pos)
{
    vec3 result = vec3(0);

    for (int i = 0; i < number_of_point_lights; i++)
    {
        PointLight light = point_lights[i];

        // Ambient
        float ambient = terrain.ambient * light.ambient;

        // Diffuse
        vec3 ld = normalize(light.position - world_pos);
        float df = dot(normal, ld);
        float diffuse = max(df, 0.0f) * terrain.diffuse * light.diffuse;

        // Specular
        vec3 hd = normalize(ld + light_out);
        float sf = max(dot(normal, hd), 0.0);
        float specular = pow(sf, terrain.shininess) * terrain.specular * light.specular;

        // Attenuation
        float distance = length(light.position - world_pos);
        float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

        result += (ambient + diffuse + specular) * color * light.color * attenuation;
    }

    return result;
}

vec3 process_haze(float distance, vec3 color)
{
    vec3 result = color;

    if (haze.enabled)
    {
        float factor = exp(-pow(distance * 0.00005f * haze.density, haze.gradient));
        factor = clamp(factor, 0.0f, 1.0f);
        result = mix(haze.color * clamp(-directional_lights[0].direction.y, 0.0f, 1.0f), color, factor);
    }

    return result;
}

void main()
{
    vec3 out_color = vec3(0);

    float[4] draw_strengths = terrainBlending(In.world_pos, In.normal);

    vec3[4] tex_coords;
    tex_coords[0] = getTextureCoordinate(In.world_pos, 0);
    tex_coords[1] = getTextureCoordinate(In.world_pos, 1);
    tex_coords[2] = getTextureCoordinate(In.world_pos, 2);
    tex_coords[3] = getTextureCoordinate(In.world_pos, 3);

    vec3 color = vec3(0);
    vec3 normal = vec3(0);

    float weights[4] = getTextureWeightsByDisplacement(tex_coords, draw_strengths);

    for (int i = 0; i < weights.length; i++)
    {
        color += texture(albedos, tex_coords[i]).rgb * weights[i];
        normal += getWorldNormal(tex_coords[i]) * weights[i];
    }

    vec3 light_out = normalize(eye_world_pos - In.world_pos);
    float distance = length(eye_world_pos - In.world_pos);

    vec3 result = vec3(0.0f);
    result += process_directional_lights(color, normal, light_out);
    result += process_point_lights(color, normal, light_out, In.world_pos);
    result = process_haze(distance, result);

    // Note that the terrain has no model matrix so its local and world coordinates are equal.
    FragLocalPosition = vec4(In.world_pos, 1.0f);
    FragWorldPosition = vec4(In.world_pos, 1.0f);

    FragNodeInfo = vec4(node_id, 0, 0, 1);
    FragColor = vec4(result, 1.0);
    gl_FragDepth = log2(In.f_log_z) / log2(z_far + 1.0);
}
