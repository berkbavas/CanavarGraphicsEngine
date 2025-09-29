#version 450 core

struct Model
{
    vec3 Color;
    float Ambient;
    float Diffuse;
    float Specular;
    float Shininess;
    float Opacity;
};

struct DirectionalLight
{
    vec3 Color;
    vec3 Direction;
    float Ambient;
    float Diffuse;
    float Specular;
    float Radiance;
};

struct PointLight
{
    vec3 Color;
    vec3 Position;
    float Ambient;
    float Diffuse;
    float Specular;
    float Constant;
    float Linear;
    float Quadratic;
};

struct Haze
{
    vec3 Color;
    float Density;
    float Gradient;
    bool Enabled;
};

struct Shadow
{
    sampler2D Map;
    bool Enabled;
    float Bias;
    int Samples;
};

uniform PointLight uPointLights[8];
uniform int uNumberOfPointLights;

uniform DirectionalLight uDirectionalLights[8];
uniform int uNumberOfDirectionalLights;

uniform Model uModel;
uniform Haze uHaze;
uniform Shadow uShadow;
uniform vec3 uCameraPosition;
uniform float uFarPlane;

in vec3 fsLocalPosition;
in vec3 fsWorldPosition;
in vec3 fsNormal;
flat in int fsVertexId;
in float fsFlogZ;
in vec4 fsLightSpacePosition; // Position in light space

uniform int uNodeId;

vec3 ProcessDirectionalLights(vec3 Color, vec3 Normal, vec3 Lo, float ShadowFactor)
{
    vec3 Result = vec3(0.0f);

    for (int i = 0; i < uNumberOfDirectionalLights; i++)
    {
        DirectionalLight Light = uDirectionalLights[i];

        // Ambient
        float Ambient = uModel.Ambient * Light.Ambient;

        // Diffuse
        vec3 Ld = -Light.Direction;
        float Df = dot(Normal, Ld);
        float Diffuse = max(Df, 0.0f) * uModel.Diffuse * Light.Diffuse;

        // Specular
        vec3 Hd = normalize(Ld + Lo);
        float Sf = max(dot(Normal, Hd), 0.0);
        float Specular = pow(Sf, uModel.Shininess) * uModel.Specular * Light.Specular;
        float LightFactor = (Ambient * (1.0f - 0.5 * ShadowFactor) + Diffuse * (1.0f - ShadowFactor) + Specular * (1.0f - ShadowFactor));
        Result += LightFactor * Color * Light.Color;
    }

    return Result;
}

vec3 ProcessPointLights(vec3 Color, vec3 Normal, vec3 Lo, vec3 FragWorldPos, float ShadowFactor)
{
    vec3 Result = vec3(0);

    for (int i = 0; i < uNumberOfPointLights; i++)
    {
        PointLight Light = uPointLights[i];

        // Ambient
        float Ambient = uModel.Ambient * Light.Ambient;

        // Diffuse
        vec3 Ld = normalize(Light.Position - FragWorldPos);
        float Df = dot(Normal, Ld);
        float Diffuse = max(Df, 0.0f) * uModel.Diffuse * Light.Diffuse;

        // Specular
        vec3 Hd = normalize(Ld + Lo);
        float Sf = max(dot(Normal, Hd), 0.0);
        float Specular = pow(Sf, uModel.Shininess) * uModel.Specular * Light.Specular;

        // Attenuation
        float Distance = length(Light.Position - FragWorldPos);
        float Attenuation = 1.0f / (Light.Constant + Light.Linear * Distance + Light.Quadratic * (Distance * Distance));
        float LightFactor = (Ambient * (1.0f - 0.5 * ShadowFactor) + Diffuse * (1.0f - ShadowFactor) + Specular * (1.0f - ShadowFactor));
        Result += LightFactor * Color * Light.Color * Attenuation;
    }

    return Result;
}

vec3 ProcessHaze(float Distance, vec3 FragWorldPos, vec3 SubjectColor)
{
    vec3 Result = SubjectColor;

    if (uHaze.Enabled)
    {
        float Factor = exp(-pow(Distance * 0.00005f * uHaze.Density, uHaze.Gradient));
        Factor = clamp(Factor, 0.0f, 1.0f);
        Result = mix(uHaze.Color * clamp(-uDirectionalLights[0].Direction.y, 0.0f, 1.0f), SubjectColor, Factor);
    }

    return Result;
}

// Reference: https://learnopengl.com/code_viewer_gh.php?code=src/5.advanced_lighting/3.1.3.shadow_mapping/3.1.3.shadow_mapping.fs
float CalculateShadow()
{
    float Result = 0.0f;

    if (uShadow.Enabled)
    {
        // perform perspective divide
        vec3 ProjCoords = fsLightSpacePosition.xyz / fsLightSpacePosition.w;

        // transform to [0,1] range
        ProjCoords = ProjCoords * 0.5 + 0.5;

        // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
        float ClosestDepth = texture(uShadow.Map, ProjCoords.xy).r;

        // get depth of current fragment from light's perspective
        float CurrentDepth = ProjCoords.z;

        vec2 TexelSize = 1.0 / textureSize(uShadow.Map, 0);

        int Samples = uShadow.Samples;

        for (int x = -Samples; x <= Samples; ++x)
        {
            for (int y = -Samples; y <= Samples; ++y)
            {
                float PcfDepth = texture(uShadow.Map, ProjCoords.xy + vec2(x, y) * TexelSize).r;
                Result += CurrentDepth - uShadow.Bias > PcfDepth ? 1.0 : 0.0;
            }
        }

        Result /= pow(2 * Samples + 1, 2);

        // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
        if (ProjCoords.z > 1.0)
        {
            Result = 0.0;
        }
    }

    return Result;
}

layout(location = 0) out vec4 OutFragColor;
layout(location = 1) out vec4 OutFragLocalPosition;
layout(location = 2) out vec4 OutFragWorldPosition;
layout(location = 3) out vec4 OutNodeInfo;

void main()
{
    float ShadowFactor = CalculateShadow();
    vec3 Lo = normalize(uCameraPosition - fsWorldPosition);
    float Distance = length(uCameraPosition - fsWorldPosition);
    vec3 Normal = normalize(fsNormal);
    vec3 Color = uModel.Color;
    float Opacity = uModel.Opacity;

    // Process
    vec3 Result = ProcessDirectionalLights(Color, Normal, Lo, ShadowFactor);
    Result += ProcessPointLights(Color, Normal, Lo, fsWorldPosition, ShadowFactor);

    // Final
    Result = ProcessHaze(Distance, fsWorldPosition, Result);

    OutFragColor = vec4(Result, Opacity);

    // Fragment position
    OutFragLocalPosition = vec4(fsLocalPosition, 1.0f);
    OutFragWorldPosition = vec4(fsWorldPosition, 1.0f);

    // Node Info
    OutNodeInfo = vec4(float(uNodeId), -1.0f, float(gl_PrimitiveID), 1.0f);

    gl_FragDepth = log2(fsFlogZ) / log2(uFarPlane + 1.0);
}
