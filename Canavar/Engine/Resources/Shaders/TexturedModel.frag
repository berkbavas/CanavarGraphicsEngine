#version 430 core

struct PhongMaterial
{
    float Ambient;
    float Diffuse;
    float Specular;
    float Shininess;
};

struct PbrMaterial
{
    float Metallic;
    float Roughness;
    float AmbientOcclusion;
};

struct Texture
{
    sampler2D BaseColor;
    sampler2D Normal;
    sampler2D Roughness;
    sampler2D Metallic;
    sampler2D AmbientOcclusion;

    bool HasBaseColor;
    bool HasNormal;
    bool HasRoughness;
    bool HasMetallic;
    bool HasAmbientOcclusion;
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

// Constants
const float PI = 3.141592653;
const float ONE_OVER_PI = 0.31830988618;

// Shading modes
const int PBR_SHADING = 0;
const int PHONG_SHADING = 1;

// Uniforms
uniform PhongMaterial uPhongMaterial;
uniform PbrMaterial uPbrMaterial;
uniform Texture uTexture;
uniform vec3 uColor;
uniform bool uUseColor;
uniform float uOpacity;
uniform int uShadingMode;
uniform vec3 uCameraPosition;
uniform DirectionalLight uDirectionalLights[8];
uniform PointLight uPointLights[8];
uniform int uNumPointLights;
uniform int uNumDirectionalLights;
uniform int uNodeId;
uniform int uMeshId;
uniform float uFar;

// Inputs from the vertex shader
in vec3 fsFragWorldPosition;
in vec3 fsFragLocalPosition;
in vec3 fsNormal;
in vec2 fsTextureCoords;
in mat3 fsTBN;
flat in int fsVertexId;
in float fsFlogZ;

// Outputs to the framebuffer
layout(location = 0) out vec4 OutFragColor;
layout(location = 1) out vec4 OutFragLocalPosition;
layout(location = 2) out vec4 OutFragWorldPosition;
layout(location = 3) out vec4 OutNodeInfo;

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

vec3 CalculateColor()
{
    vec3 Color;

    if (uUseColor)
    {
        Color = uColor;
    }
    else if (uTexture.HasBaseColor)
    {
        Color = texture(uTexture.BaseColor, fsTextureCoords).rgb;
    }
    else
    {
        Color = uColor;
    }

    return Color;
}

vec3 CalculateNormal()
{
    vec3 Normal;

    if (uTexture.HasNormal)
    {
        Normal = texture(uTexture.Normal, fsTextureCoords).rgb;
        Normal = 2.0 * Normal - 1.0;
        Normal = fsTBN * Normal;
    }
    else
    {
        Normal = fsNormal;
    }

    return normalize(Normal);
}

float CalculateMetallic()
{
    float Metallic;

    if (uTexture.HasMetallic)
    {
        Metallic = texture(uTexture.Metallic, fsTextureCoords).r;
    }
    else
    {
        Metallic = uPbrMaterial.Metallic;
    }

    return Metallic;
}

float CalculateRoughness()
{
    float Roughness;

    if (uTexture.HasRoughness)
    {
        Roughness = texture(uTexture.Roughness, fsTextureCoords).r;
    }
    else
    {
        Roughness = uPbrMaterial.Roughness;
    }

    return Roughness;
}

float CalculateAmbientOcclusion()
{
    float AmbientOcclusion;

    if (uTexture.HasAmbientOcclusion)
    {
        AmbientOcclusion = texture(uTexture.AmbientOcclusion, fsTextureCoords).r;
    }
    else
    {
        AmbientOcclusion = uPbrMaterial.AmbientOcclusion;
    }

    return AmbientOcclusion;
}

vec3 ProcessDirectionalLightsPhong(vec3 Color, vec3 Normal, vec3 Lo)
{
    vec3 Result = vec3(0.0f);

    for (int i = 0; i < uNumDirectionalLights; i++)
    {
        DirectionalLight Light = uDirectionalLights[i];

        // Ambient
        float Ambient = uPhongMaterial.Ambient * Light.Ambient;

        // Diffuse
        vec3 Ld = -Light.Direction;
        float Df = dot(Normal, Ld);
        float Diffuse = max(Df, 0.0f) * uPhongMaterial.Diffuse * Light.Diffuse;

        // Specular
        vec3 Hd = normalize(Ld + Lo);
        float Sf = max(dot(Normal, Hd), 0.0);
        float Specular = pow(Sf, uPhongMaterial.Shininess) * uPhongMaterial.Specular * Light.Specular;

        // Attenuation
        float LightFactor = Ambient + Diffuse + Specular;

        // Accumulate the result
        Result += LightFactor * Color * Light.Color;
    }

    return Result;
}

vec3 ProcessPointLightsPhong(vec3 Color, vec3 Normal, vec3 Lo)
{
    vec3 Result = vec3(0.0f);

    for (int i = 0; i < uNumPointLights; i++)
    {
        PointLight Light = uPointLights[i];

        // Ambient
        float Ambient = uPhongMaterial.Ambient * Light.Ambient;

        // Diffuse
        vec3 Ld = normalize(Light.Position - fsFragWorldPosition);
        float Df = dot(Normal, Ld);
        float Diffuse = max(Df, 0.0f) * uPhongMaterial.Diffuse * Light.Diffuse;

        // Specular
        vec3 Hd = normalize(Ld + Lo);
        float Sf = max(dot(Normal, Hd), 0.0);
        float Specular = pow(Sf, uPhongMaterial.Shininess) * uPhongMaterial.Specular * Light.Specular;

        // Attenuation
        float Distance = length(Light.Position - fsFragWorldPosition);
        float Attenuation = 1.0f / (Light.Constant + Light.Linear * Distance + Light.Quadratic * (Distance * Distance));

        // Combine the components
        float LightFactor = Ambient + Diffuse + Specular;

        // Accumulate the result
        Result += LightFactor * Color * Light.Color * Attenuation;
    }

    return Result;
}

vec3 ProcessDirectionalLightsPbr(vec3 Color, float Metallic, float Roughness, float AmbientOcclusion, vec3 Normal, vec3 Lo)
{
    vec3 F0 = mix(vec3(0.04f), Color, Metallic);

    vec3 Result = vec3(0.0);

    for (int i = 0; i < uNumDirectionalLights; ++i)
    {
        DirectionalLight Light = uDirectionalLights[i];

        vec3 Li = -Light.Direction;
        vec3 H = normalize(Lo + Li);
        vec3 Radiance = Light.Color * Light.Radiance;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(Normal, H, Roughness);
        float G = GeometrySmith(Normal, Lo, Li, Roughness);
        vec3 F = FresnelSchlick(max(dot(H, Lo), 0.0f), F0);

        vec3 Nominator = NDF * G * F;
        float Denominator = 4.0 * max(dot(Normal, Lo), 0.0f) * max(dot(Normal, Li), 0.0f) + 0.001f;
        vec3 Specular = Nominator / Denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0f) - kS;
        kD *= 1.0f - Metallic;

        float NdotL = max(dot(Normal, Li), 0.0f);
        Result += (kD * Color / PI + Specular) * Radiance * NdotL;
    }

    const vec3 Ambient = Color * AmbientOcclusion;
    const vec3 FinalColor = Ambient + Result;

    return FinalColor;
}

vec3 ProcessPointLightsPbr(vec3 Color, float Metallic, float Roughness, float AmbientOcclusion, vec3 Normal, vec3 Lo)
{
    vec3 F0 = mix(vec3(0.04f), Color, Metallic);

    vec3 Result = vec3(0.0);

    for (int i = 0; i < uNumPointLights; ++i)
    {
        PointLight Light = uPointLights[i];

        vec3 Li = normalize(Light.Position - fsFragWorldPosition);
        vec3 H = normalize(Lo + Li);
        float Distance = length(Light.Position - fsFragWorldPosition);
        float Attenuation = 1.0f / (Light.Constant + Light.Linear * Distance + Light.Quadratic * (Distance * Distance));
        vec3 Radiance = Light.Color * Attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(Normal, H, Roughness);
        float G = GeometrySmith(Normal, Lo, Li, Roughness);
        vec3 F = FresnelSchlick(max(dot(H, Lo), 0.0f), F0);

        vec3 Nominator = NDF * G * F;
        float Denominator = 4.0 * max(dot(Normal, Lo), 0.0f) * max(dot(Normal, Li), 0.0f) + 0.001f;
        vec3 Specular = Nominator / Denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0f) - kS;
        kD *= 1.0f - Metallic;

        float NdotL = max(dot(Normal, Li), 0.0f);
        Result += (kD * Color / PI + Specular) * Radiance * NdotL;
    }

    const vec3 Ambient = Color * AmbientOcclusion;
    const vec3 FinalColor = Ambient + Result;

    return FinalColor;
}

void main()
{
    const vec3 Color = CalculateColor();
    const vec3 Normal = CalculateNormal();
    const vec3 Lo = normalize(uCameraPosition - fsFragWorldPosition);

    vec4 FragColor;

    if (uShadingMode == PHONG_SHADING)
    {
        const vec3 DirectLighting = ProcessDirectionalLightsPhong(Color, Normal, Lo);
        const vec3 PointLighting = ProcessPointLightsPhong(Color, Normal, Lo);
        const vec3 FinalColor = DirectLighting + PointLighting;

        FragColor = vec4(FinalColor, uOpacity);
    }
    else if (uShadingMode == PBR_SHADING)
    {
        const float Metallic = CalculateMetallic();
        const float Roughness = CalculateRoughness();
        const float AmbientOcclusion = CalculateAmbientOcclusion();

        const vec3 DirectLighting = ProcessDirectionalLightsPbr(Color, Metallic, Roughness, AmbientOcclusion, Normal, Lo);
        const vec3 PointLighting = ProcessPointLightsPbr(Color, Metallic, Roughness, AmbientOcclusion, Normal, Lo);
        const vec3 FinalColor = DirectLighting + PointLighting;

        FragColor = vec4(FinalColor, uOpacity);
    }
    else
    {
        FragColor = vec4(Color, uOpacity); // Fallback to base color if shading mode is not recognized
    }

    OutFragColor = vec4(clamp(FragColor.rgb, 0.0f, 1.0f), FragColor.a);
    OutFragLocalPosition = vec4(fsFragLocalPosition, 1.0f);
    OutFragWorldPosition = vec4(fsFragWorldPosition, 1.0f);
    OutNodeInfo = vec4(float(uNodeId), float(uMeshId), float(gl_PrimitiveID), 1.0f);

    gl_FragDepth = log2(fsFlogZ) / log2(uFar + 1.0f);
}
