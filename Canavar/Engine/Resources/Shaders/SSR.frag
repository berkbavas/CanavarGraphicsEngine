#version 450 core

uniform sampler2D uSceneColor;
uniform sampler2D uDepthTex;
uniform sampler2D uNormalTex;
uniform sampler2D uMaterialTex;

uniform mat4 uProj;
uniform mat4 uInvProj;
uniform mat3 uViewNormalMatrix;

uniform float uMaxDistance;
uniform int uMaxSteps;
uniform float uThickness;
uniform float uStride;
uniform vec2 uResolution;

in vec2 fsTextureCoords;

layout(location = 0) out vec4 oColor;

vec3 ReconstructViewPos(vec2 uv, float depth01)
{
    float z = depth01 * 2.0 - 1.0;
    vec4 clip = vec4(uv * 2.0 - 1.0, z, 1.0);
    vec4 view = uInvProj * clip;
    return view.xyz / view.w;
}

float DepthFetch(vec2 uv)
{
    return texture(uDepthTex, uv).z;
}

vec3 ViewToScreen(vec3 viewPos)
{
    vec4 clip = uProj * vec4(viewPos, 1.0);
    vec3 ndc = clip.xyz / clip.w;
    return vec3(ndc.xy * 0.5 + 0.5, viewPos.z);
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
    vec2 uv = fsTextureCoords;
    float depth01 = DepthFetch(uv);
    if (depth01 >= 1.0)
    {
        oColor = vec4(0, 0, 0, 0);
        return;
    }

    vec3 P = ReconstructViewPos(uv, depth01);

    // world -> view normal
    vec3 Nworld = normalize(texture(uNormalTex, uv).xyz);
    vec3 N = normalize(uViewNormalMatrix * Nworld);

    // material params
    vec3 mat = texture(uMaterialTex, uv).rgb;
    float roughness = clamp(mat.r, 0.0, 1.0);
    float metallic = clamp(mat.g, 0.0, 1.0);
    float ao = clamp(mat.b, 0.0, 1.0);

    vec3 V = normalize(-P);
    vec3 R = normalize(reflect(-V, N));

    // marching setup
    vec3 marchPos = P + R * 0.05;
    vec3 marchScreen = ViewToScreen(marchPos);
    vec2 dir = normalize(marchScreen.xy - uv);
    vec2 pixelSize = 1.0 / uResolution;
    vec2 stepUV = dir * max(1.0, uStride) * pixelSize;

    vec3 hitColor = vec3(0.0);
    float hitMask = 0.0;
    float traveled = 0.0;
    vec2 sampleUV = uv;

    for (int steps = 0; steps < uMaxSteps; ++steps)
    {
        sampleUV += stepUV;

        if (any(lessThan(sampleUV, vec2(0.0))) || any(greaterThan(sampleUV, vec2(1.0))))
            break;

        float d = DepthFetch(sampleUV);
        if (d >= 1.0)
        {
            traveled += length(stepUV) * 2.0;
            continue;
        }

        vec3 scenePos = ReconstructViewPos(sampleUV, d);
        float dz = scenePos.z - marchPos.z;

        if (dz > -uThickness && dz < uThickness)
        {
            // refine
            vec2 lowUV = sampleUV - stepUV;
            vec2 highUV = sampleUV;
            for (int i = 0; i < 5; i++)
            {
                vec2 midUV = 0.5 * (lowUV + highUV);
                float md = DepthFetch(midUV);
                vec3 mp = ReconstructViewPos(midUV, md);
                if (mp.z > marchPos.z)
                    highUV = midUV;
                else
                    lowUV = midUV;
            }
            vec2 hitUV = 0.5 * (lowUV + highUV);

            hitColor = texture(uSceneColor, hitUV).rgb;

            // ---- Fresnel with metallic ----
            vec3 F0 = mix(vec3(0.04), clamp(hitColor, 0.0, 1.0), metallic);
            float NdotV = max(dot(N, V), 0.0);
            vec3 F = FresnelSchlick(NdotV, F0);

            // Glossy = (1-roughness)^2; AO attenuates SSR contribution pragmatically
            float glossy = (1.0 - roughness);
            glossy *= glossy;

            // Final SSR mask: glossy * average Fresnel * AO
            float Favg = (F.r + F.g + F.b) * (1.0 / 3.0);
            hitMask = clamp(glossy * Favg * ao, 0.0, 1.0);
            break;
        }

        marchPos += R * 0.1;
        traveled += 0.1;
        if (traveled > uMaxDistance)
            break;
    }

    // tiny roughness-aware blur (optional)
    if (hitMask > 0.0 && roughness > 0.0)
    {
        vec2 r = roughness * 2.0 / uResolution;
        hitColor = (hitColor + texture(uSceneColor, fsTextureCoords + vec2(r.x, 0)).rgb //
                    + texture(uSceneColor, fsTextureCoords + vec2(-r.x, 0)).rgb         //
                    + texture(uSceneColor, fsTextureCoords + vec2(0, r.y)).rgb          //
                    + texture(uSceneColor, fsTextureCoords + vec2(0, -r.y)).rgb)
                   / 5.0;
    }

    oColor = vec4(hitColor, hitMask);
}