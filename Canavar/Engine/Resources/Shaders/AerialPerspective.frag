#version 450 core

// Aerial perspective: applies the same Rayleigh+Mie atmosphere model as the sky
// to scene objects based on their world-space distance from the camera.

uniform sampler2D uSceneTexture;
uniform sampler2D uWorldPosTexture; // COLOR_ATTACHMENT2: rgb=worldPos, a=nodeId (0 = sky)

uniform vec3 uCameraPos;
uniform vec3 uSunPos; // normalized direction toward the sun
uniform float uSunIntensity;
uniform float uPlanetRadius;
uniform float uAtmosphereRadius;
uniform vec3 uBetaRayleigh;
uniform float uBetaMie;
uniform float uScaleHeightR;
uniform float uScaleHeightM;
uniform float uMieG;
uniform float uDensity; // distance scale; try 1e-5 for km-scale scenes

in vec2 fsTextureCoords;
out vec4 oFragColor;

#define PI 3.14159265
#define ISteps 8
#define JSteps 4

vec2 Rsi(vec3 R0, vec3 Rd, float Sr)
{
    float A = dot(Rd, Rd);
    float B = 2.0 * dot(Rd, R0);
    float C = dot(R0, R0) - Sr * Sr;
    float D = B * B - 4.0 * A * C;
    if (D < 0.0)
        return vec2(1e5, -1e5);
    return vec2((-B - sqrt(D)) / (2.0 * A), (-B + sqrt(D)) / (2.0 * A));
}

vec3 Atmosphere(vec3 R, vec3 R0, vec3 PSun, float ISun, float RPlanet, float RAtmos, vec3 KRlh, float KMie, float ShRlh, float ShMie, float G)
{
    PSun = normalize(PSun);
    R = normalize(R);

    vec2 P = Rsi(R0, R, RAtmos);
    if (P.x > P.y)
        return vec3(0.0);
    P.y = min(P.y, Rsi(R0, R, RPlanet).x);
    float IStepSize = (P.y - P.x) / float(ISteps);

    float ITime = 0.0;
    vec3 TotalRlh = vec3(0.0);
    vec3 TotalMie = vec3(0.0);
    float IOdRlh = 0.0;
    float IOdMie = 0.0;

    float Mu = dot(R, PSun);
    float Mumu = Mu * Mu;
    float Gg = G * G;
    float PRlh = 3.0 / (16.0 * PI) * (1.0 + Mumu);
    float PMie = 3.0 / (8.0 * PI) * ((1.0 - Gg) * (Mumu + 1.0)) / (pow(1.0 + Gg - 2.0 * Mu * G, 1.5) * (2.0 + Gg));

    for (int I = 0; I < ISteps; I++)
    {
        vec3 IPos = R0 + R * (ITime + IStepSize * 0.5);
        float IHeight = length(IPos) - RPlanet;
        float OdStepRlh = exp(-IHeight / ShRlh) * IStepSize;
        float OdStepMie = exp(-IHeight / ShMie) * IStepSize;
        IOdRlh += OdStepRlh;
        IOdMie += OdStepMie;

        float JStepSize = Rsi(IPos, PSun, RAtmos).y / float(JSteps);
        float JTime = 0.0;
        float JOdRlh = 0.0;
        float JOdMie = 0.0;
        for (int J = 0; J < JSteps; J++)
        {
            vec3 JPos = IPos + PSun * (JTime + JStepSize * 0.5);
            float JHeight = length(JPos) - RPlanet;
            JOdRlh += exp(-JHeight / ShRlh) * JStepSize;
            JOdMie += exp(-JHeight / ShMie) * JStepSize;
            JTime += JStepSize;
        }

        vec3 Attn = exp(-(KMie * (IOdMie + JOdMie) + KRlh * (IOdRlh + JOdRlh)));
        TotalRlh += OdStepRlh * Attn;
        TotalMie += OdStepMie * Attn;
        ITime += IStepSize;
    }

    return ISun * (PRlh * KRlh * TotalRlh + PMie * KMie * TotalMie);
}

void main()
{
    vec4 WorldPosSample = texture(uWorldPosTexture, fsTextureCoords);
    vec3 SceneColor = texture(uSceneTexture, fsTextureCoords).rgb;

    // Sky pixels have alpha = 0; skip them (sky already contains atmosphere)
    if (WorldPosSample.a < 0.5)
    {
        oFragColor = vec4(SceneColor, 1.0);
        return;
    }

    vec3 WorldPos = WorldPosSample.rgb;
    float Distance = length(WorldPos - uCameraPos);
    vec3 ViewDir = (WorldPos - uCameraPos) / Distance;

    // Atmosphere inscattering color along this view direction
    vec3 AtmoColor = Atmosphere(ViewDir, //
                                uCameraPos + vec3(0.0, uPlanetRadius, 0.0),
                                uSunPos,
                                uSunIntensity,
                                uPlanetRadius,
                                uAtmosphereRadius,
                                uBetaRayleigh,
                                uBetaMie,
                                uScaleHeightR,
                                uScaleHeightM,
                                uMieG);
    AtmoColor = 1.0 - exp(-AtmoColor); // same exposure as Sky.frag

    // Transmittance: how much of the atmosphere accumulates with distance
    float T = 1.0 - exp(-uDensity * Distance);
    T = clamp(T, 0.0, 1.0);

    oFragColor = vec4(mix(SceneColor, AtmoColor, T), 1.0);
}
