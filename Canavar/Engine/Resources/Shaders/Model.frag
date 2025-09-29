#version 450 core

struct Model
{
    vec3 Color;
    vec3 TransparencyColor;
    float Ambient;
    float Diffuse;
    float Specular;
    float Shininess;
    float Metallic;
    float Roughness;
    float AmbientOcclusion;
    bool UseModelColor;
    int ShadingMode;
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

uniform Haze uHaze;
uniform Model uModel;
uniform Shadow uShadow;

uniform PointLight uPointLights[8];
uniform int uNumberOfPointLights;

uniform DirectionalLight uDirectionalLights[8];
uniform int uNumberOfDirectionalLights;

uniform vec3 uCameraPosition;
uniform float uFarPlane;

uniform float uMeshOpacity;

uniform sampler2D uTextureBaseColor;
uniform sampler2D uTextureMetallic;
uniform sampler2D uTextureRoughness;
uniform sampler2D uTextureAmbientOcclusion;
uniform sampler2D uTextureNormal;

uniform bool uHasTextureBaseColor;
uniform bool uHasTextureMetallic;
uniform bool uHasTextureRoughness;
uniform bool uHasTextureAmbientOcclusion;
uniform bool uHasTextureNormal;

uniform int uNodeId;
uniform int uMeshId;
uniform int uSelectedMeshId;
uniform int uLightSpaceMatrixId;

uniform int uMeshSelectionEnabled;

in vec3 fsLocalPosition;
in vec3 fsWorldPosition;
in vec3 fsNormal;
in vec2 fsTextureCoords;
in mat3 fsTBN;
flat in int fsVertexId;
in float fsFlogZ;
in vec4 fsLightSpacePosition; // Position in light space
in vec3 fsColor;
in flat uint fsMask;

layout(location = 0) out vec4 OutFragColor;
layout(location = 1) out vec4 OutFragLocalPosition;
layout(location = 2) out vec4 OutFragWorldPosition;
layout(location = 3) out vec4 OutNodeInfo;

const uint NO_MASK = 0;
const uint PAINTED_MASK = 1;

const float EPSILON = 0.00001f;
const float PI = 3.14159265359;

const vec3 RED = vec3(1.0f, 0.0f, 0.0f);
const vec3 GREEN = vec3(0.0f, 1.0f, 0.0f);
const vec3 BLUE = vec3(0.0f, 0.0f, 1.0f);

const int PBR_SHADING = 0;
const int PHONG_SHADING = 1;

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float Roughness)
{
    float A = Roughness * Roughness;
    float A2 = A * A;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float Denom = (NdotH2 * (A2 - 1.0) + 1.0);
    Denom = PI * Denom * Denom;

    return A2 / Denom;
}

float GeometrySchlickGGX(float NdotV, float Roughness)
{
    float r = Roughness + 1.0;
    float k = (r * r) / 8.0;

    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float Roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, Roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, Roughness);
    return ggx1 * ggx2;
}

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

vec3 ProcessDirectionalLightsPbr(vec3 Albedo, float Metallic, float Roughness, float AmbientOcclusion, vec3 N /* Normal */, vec3 Lo, float ShadowFactor)
{
    vec3 F0 = mix(vec3(0.04f), Albedo, Metallic);

    vec3 Result = vec3(0.0);

    for (int i = 0; i < uNumberOfDirectionalLights; ++i)
    {
        DirectionalLight Light = uDirectionalLights[i];

        vec3 Li = -Light.Direction;
        vec3 H = normalize(Lo + Li);
        vec3 Radiance = Light.Color * Light.Radiance;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, Roughness);
        float G = GeometrySmith(N, Lo, Li, Roughness);
        vec3 F = FresnelSchlick(max(dot(H, Lo), 0.0f), F0);

        vec3 Nominator = NDF * G * F;
        float Denominator = 4.0 * max(dot(N, Lo), 0.0f) * max(dot(N, Li), 0.0f) + 0.001f;
        vec3 Specular = Nominator / Denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0f) - kS;
        kD *= 1.0f - Metallic;

        float NdotL = max(dot(N, Li), 0.0f);
        Result += (kD * Albedo / PI + Specular) * Radiance * NdotL;
    }

    // Ambient (no IBL, just constant AO term)
    vec3 Ambient = vec3(0.03f) * Albedo * AmbientOcclusion;
    vec3 Color = Ambient * (1 - 0.5 * ShadowFactor) + Result * (1 - ShadowFactor);

    return Color;
}

vec3 ProcessPointLightsPbr(vec3 Albedo, float Metallic, float Roughness, float AmbientOcclusion, vec3 N /* Normal */, vec3 Lo, vec3 FragWorldPos, float ShadowFactor)
{
    vec3 F0 = mix(vec3(0.04f), Albedo, Metallic);

    vec3 Result = vec3(0.0);

    for (int i = 0; i < uNumberOfPointLights; ++i)
    {
        PointLight Light = uPointLights[i];

        vec3 Li = normalize(Light.Position - FragWorldPos);
        vec3 H = normalize(Lo + Li);
        float Distance = length(Light.Position - FragWorldPos);
        float Attenuation = 1.0f / (Light.Constant + Light.Linear * Distance + Light.Quadratic * (Distance * Distance));
        vec3 Radiance = Light.Color * Attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, Roughness);
        float G = GeometrySmith(N, Lo, Li, Roughness);
        vec3 F = FresnelSchlick(max(dot(H, Lo), 0.0f), F0);

        vec3 Nominator = NDF * G * F;
        float Denominator = 4.0 * max(dot(N, Lo), 0.0f) * max(dot(N, Li), 0.0f) + 0.001f;
        vec3 Specular = Nominator / Denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0f) - kS;
        kD *= 1.0f - Metallic;

        float NdotL = max(dot(N, Li), 0.0f);
        Result += (kD * Albedo / PI + Specular) * Radiance * NdotL;
    }

    // Ambient (no IBL, just constant AO term)
    vec3 Ambient = vec3(0.03f) * Albedo * AmbientOcclusion;
    vec3 Color = Ambient * (1 - 0.5 * ShadowFactor) + Result * (1 - ShadowFactor);

    return Color;
}

float CalculateMetallic()
{
    float Metallic;

    if (uHasTextureMetallic)
    {
        Metallic = texture(uTextureMetallic, fsTextureCoords).r;
    }
    else
    {
        Metallic = uModel.Metallic;
    }

    return Metallic;
}

float CalculateRoughness()
{
    float Roughness;

    if (uHasTextureRoughness)
    {
        Roughness = texture(uTextureRoughness, fsTextureCoords).r;
    }
    else
    {
        Roughness = uModel.Roughness;
    }

    return Roughness;
}

float CalculateAmbientOcclusion()
{
    float AmbientOcclusion;

    if (uHasTextureAmbientOcclusion)
    {
        AmbientOcclusion = texture(uTextureAmbientOcclusion, fsTextureCoords).r;
    }
    else
    {
        AmbientOcclusion = uModel.AmbientOcclusion;
    }

    return AmbientOcclusion;
}

vec3 CalculateNormal()
{
    vec3 Normal;

    if (uHasTextureNormal)
    {
        Normal = texture(uTextureNormal, fsTextureCoords).xyz;
        Normal = 2.0 * Normal - 1.0;
        Normal = normalize(fsTBN * Normal);
    }
    else
    {
        Normal = fsNormal;
    }

    return Normal;
}

vec3 CalculateColor(float Opacity)
{
    vec3 Color;

    if (uModel.UseModelColor)
    {
        if (Opacity < 1.0f)
        {
            Color = uModel.TransparencyColor;
        }
        else
        {
            Color = uModel.Color;
        }
    }
    else
    {
        if (uHasTextureBaseColor)
        {
            Color = texture(uTextureBaseColor, fsTextureCoords).rgb;
        }
        else
        {
            if (Opacity < 1.0f)
            {
                Color = uModel.TransparencyColor;
            }
            else
            {
                Color = uModel.Color;
            }
        }
    }

    if (fsMask == PAINTED_MASK)
    {
        Color = fsColor;
    }

    if (uMeshSelectionEnabled == 1)
    {
        if (uSelectedMeshId == uMeshId)
        {
            Color = mix(Color, RED, 0.5f);
        }
    }

    return Color;
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

float CalculateOpacity()
{
    float Opacity;

    if (uHasTextureBaseColor)
    {
        Opacity = texture(uTextureBaseColor, fsTextureCoords).a;
    }
    else
    {
        Opacity = uMeshOpacity;
    }

    return Opacity;
}

void main()
{
    float ShadowFactor = CalculateShadow();
    float Opacity = CalculateOpacity();
    vec3 Normal = CalculateNormal();
    vec3 Color = CalculateColor(Opacity);
    vec3 Lo = normalize(uCameraPosition - fsWorldPosition);
    float distance = length(uCameraPosition - fsWorldPosition);

    // Process
    vec3 Result = vec3(0.0f);

    if (uModel.ShadingMode == PBR_SHADING)
    {
        float Metallic = CalculateMetallic();
        float Roughness = CalculateRoughness();
        float AmbientOcclusion = CalculateAmbientOcclusion();

        Result += ProcessDirectionalLightsPbr(Color, Metallic, Roughness, AmbientOcclusion, Normal, Lo, ShadowFactor);
        Result += ProcessPointLightsPbr(Color, Metallic, Roughness, AmbientOcclusion, Normal, Lo, fsWorldPosition, ShadowFactor);
    }
    else if (uModel.ShadingMode == PHONG_SHADING)
    {
        Result += ProcessDirectionalLights(Color, Normal, Lo, ShadowFactor);
        Result += ProcessPointLights(Color, Normal, Lo, fsWorldPosition, ShadowFactor);
    }

    // Final
    Result = ProcessHaze(distance, fsWorldPosition, Result);

    OutFragColor = vec4(Result, Opacity);

    // Fragment position
    OutFragLocalPosition = vec4(fsLocalPosition, 1.0f);
    OutFragWorldPosition = vec4(fsWorldPosition, 1.0f);

    // Node Info
    OutNodeInfo = vec4(float(uNodeId), float(uMeshId), float(gl_PrimitiveID), 1.0f);

    gl_FragDepth = log2(fsFlogZ) / log2(uFarPlane + 1.0);
}
