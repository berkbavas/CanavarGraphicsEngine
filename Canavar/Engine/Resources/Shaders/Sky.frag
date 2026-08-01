#version 330 core

uniform vec3 uSunPos;
uniform vec3 uCameraPos;
uniform float uSunIntensity;
uniform float uPlanetRadius;
uniform float uAtmosphereRadius;
uniform vec3 uBetaRayleigh;  // Rayleigh scattering coefficient
uniform float uBetaMie;      // Mie scattering coefficient
uniform float uScaleHeightR; // Rayleigh scale height
uniform float uScaleHeightM; // Mie scale height
uniform float uMieG;         // Mie preferred scattering direction
uniform int uNodeId;

in vec3 fsViewDir;

layout(location = 0) out vec4 oFragColor;
layout(location = 1) out vec4 oFragLocalPosition;
layout(location = 2) out vec4 oFragWorldPosition;
layout(location = 3) out vec4 oNodeInfo;

#define PI 3.14159265
#define ISteps 16
#define JSteps 8

vec2 Rsi(vec3 R0, vec3 Rd, float Sr)
{
    // ray-sphere intersection that assumes
    // the sphere is centered at the origin.
    // No intersection when result.x > result.y
    float A = dot(Rd, Rd);
    float B = 2.0 * dot(Rd, R0);
    float C = dot(R0, R0) - (Sr * Sr);
    float D = (B * B) - 4.0 * A * C;
    if (D < 0.0)
        return vec2(1e5, -1e5);
    return vec2((-B - sqrt(D)) / (2.0 * A), (-B + sqrt(D)) / (2.0 * A));
}

vec3 Atmosphere(vec3 R, vec3 R0, vec3 PSun, float ISun, float RPlanet, float RAtmos, vec3 KRlh, float KMie, float ShRlh, float ShMie, float G)
{
    // Normalize the sun and view directions.
    PSun = normalize(PSun);
    R = normalize(R);

    // Calculate the step size of the primary ray.
    vec2 P = Rsi(R0, R, RAtmos);
    if (P.x > P.y)
        return vec3(0, 0, 0);
    P.y = min(P.y, Rsi(R0, R, RPlanet).x);
    float IStepSize = (P.y - P.x) / float(ISteps);

    // Initialize the primary ray time.
    float ITime = 0.0;

    // Initialize accumulators for Rayleigh and Mie scattering.
    vec3 TotalRlh = vec3(0, 0, 0);
    vec3 TotalMie = vec3(0, 0, 0);

    // Initialize optical depth accumulators for the primary ray.
    float IOdRlh = 0.0;
    float IOdMie = 0.0;

    // Calculate the Rayleigh and Mie phases.
    float Mu = dot(R, PSun);
    float Mumu = Mu * Mu;
    float Gg = G * G;
    float PRlh = 3.0 / (16.0 * PI) * (1.0 + Mumu);
    float PMie = 3.0 / (8.0 * PI) * ((1.0 - Gg) * (Mumu + 1.0)) / (pow(1.0 + Gg - 2.0 * Mu * G, 1.5) * (2.0 + Gg));

    // Sample the primary ray.
    for (int I = 0; I < ISteps; I++)
    {
        // Calculate the primary ray sample position.
        vec3 IPos = R0 + R * (ITime + IStepSize * 0.5);

        // Calculate the height of the sample.
        float IHeight = length(IPos) - RPlanet;

        // Calculate the optical depth of the Rayleigh and Mie scattering for this step.
        float OdStepRlh = exp(-IHeight / ShRlh) * IStepSize;
        float OdStepMie = exp(-IHeight / ShMie) * IStepSize;

        // Accumulate optical depth.
        IOdRlh += OdStepRlh;
        IOdMie += OdStepMie;

        // Calculate the step size of the secondary ray.
        float JStepSize = Rsi(IPos, PSun, RAtmos).y / float(JSteps);

        // Initialize the secondary ray time.
        float JTime = 0.0;

        // Initialize optical depth accumulators for the secondary ray.
        float JOdRlh = 0.0;
        float JOdMie = 0.0;

        // Sample the secondary ray.
        for (int J = 0; J < JSteps; J++)
        {
            // Calculate the secondary ray sample position.
            vec3 JPos = IPos + PSun * (JTime + JStepSize * 0.5);

            // Calculate the height of the sample.
            float JHeight = length(JPos) - RPlanet;

            // Accumulate the optical depth.
            JOdRlh += exp(-JHeight / ShRlh) * JStepSize;
            JOdMie += exp(-JHeight / ShMie) * JStepSize;

            // Increment the secondary ray time.
            JTime += JStepSize;
        }

        // Calculate attenuation.
        vec3 Attn = exp(-(KMie * (IOdMie + JOdMie) + KRlh * (IOdRlh + JOdRlh)));

        // Accumulate scattering.
        TotalRlh += OdStepRlh * Attn;
        TotalMie += OdStepMie * Attn;

        // Increment the primary ray time.
        ITime += IStepSize;
    }

    // Calculate and return the final color.
    return ISun * (PRlh * KRlh * TotalRlh + PMie * KMie * TotalMie);
}

void main()
{
    vec3 Color = Atmosphere(fsViewDir,                              // normalized ray direction
                            uCameraPos + vec3(0, uPlanetRadius, 0), // ray origin
                            uSunPos,                                // position of the sun
                            uSunIntensity,                          // intensity of the sun
                            uPlanetRadius,                          // radius of the planet in meters
                            uAtmosphereRadius,                      // radius of the atmosphere in meters
                            uBetaRayleigh,                          // Rayleigh scattering coefficient
                            uBetaMie,                               // Mie scattering coefficient
                            uScaleHeightR,                          // Rayleigh scale height
                            uScaleHeightM,                          // Mie scale height
                            uMieG                                   // Mie preferred scattering direction
    );

    // Apply exposure.
    Color = 1.0f - exp(-1.0f * Color);

    oFragColor = vec4(Color, 1.0f);
    oFragLocalPosition = vec4(0.0f);
    oFragWorldPosition = vec4(0.0f);
    oNodeInfo = vec4(float(uNodeId), 0.0f, 0.0f, 0.0f);
}