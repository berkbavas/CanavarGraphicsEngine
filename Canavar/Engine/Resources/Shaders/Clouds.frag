#version 430 core

#define EARLY_STOP_THRESHOLD 1e-2f
#define EARLY_STOP_LOG_THRESHOLD -4.6f
#define HALF_PI 1.57079632679
#define FOUR_PI 12.5663706144
#define XZ_FALLOFF_DIST 1.f
#define Y_FALLOFF_DIST 1.f

// Params for adaptive ray marching
#define MIN_NUM_FINE_STEPS 16
#define COARSE_STEPSIZE_MULTIPLIER 4.f
#define SMALL_DENSITY 0.005f
#define MAX_NUM_MISSED_STEPS 5
#define STEPSIZE_FINE 0.02f

#define MAX_SUN_INTENSITY 4.f
#define SUN_RADIUS 100.f

// density volumes computed by the compute shader
layout(binding = 0) uniform sampler3D volumeHighRes;
layout(binding = 1) uniform sampler3D volumeLowRes;
layout(binding = 2) uniform sampler2D solidDepth;
layout(binding = 3) uniform sampler2D solidColor;
layout(binding = 4) uniform sampler1D sunGradient;
layout(binding = 5) uniform sampler2D nightColor;

//in vec3 positionWorld;
in vec2 uv;
in vec3 rayDirWorldspace;
out vec4 glFragColor;

// volume transforms for computing ray-box intersection
uniform vec3 volumeScaling, volumeTranslate;

// ray origin, updated when user moves camera
uniform vec3 rayOrigWorld;

// rendering params, updated when user changes settings
//uniform float stepSize;
uniform int numSteps;
uniform bool invertDensity, gammaCorrect;
uniform float densityMult;
uniform float cloudLightAbsorptionMult;
uniform float minLightTransmittance;

// Params for high resolution noise
uniform vec4 hiResNoiseScaling;
uniform vec3 hiResNoiseTranslate; // noise transforms
uniform vec4 hiResChannelWeights; // how to aggregate RGBA channels
uniform float hiResDensityOffset; // controls overall cloud coverage

// Params for low resolution noise
uniform float loResNoiseScaling;
uniform vec3 loResNoiseTranslate; // noise transforms
uniform vec4 loResChannelWeights; // how to aggregate RGBA channels
uniform float loResDensityWeight; // relative weight of lo-res noise about hi-res

// Camera
uniform float xMax, yMax; // rayDirWorldspace lies within [-xMax, xMax] x [-yMax, yMax] x {1.0}
uniform float near, far;  // terrain camera

// light uniforms
struct LightData
{
    int type;
    vec4 pos;
    //    vec3 dir;  // towards light source
    vec3 color;
    float longitude;
    float latitude;
};

uniform vec4 phaseParams; // HG
uniform LightData testLight;

vec3 dirSph2Cart(float latitudeRadians, float longitudeRadians)
{
    float x, y, z;
    x = sin(longitudeRadians) * sin(latitudeRadians);
    y = cos(longitudeRadians);
    z = sin(longitudeRadians) * cos(latitudeRadians);
    return vec3(x, y, z);
}

// normalized v so that dot(v, 1) = 1
vec4 normalizeL1(vec4 v)
{
    return v / dot(v, vec4(1.f));
}

// gamma correction
float linear2srgb(float x)
{
    if (x <= 0.0031308f)
        return 12.92f * x;
    return 1.055f * pow(x, 1.f / 2.4f) - 0.055f;
}

vec3 gammaCorrection(vec3 linearRGB)
{
    return vec3(linear2srgb(linearRGB.r), linear2srgb(linearRGB.g), linear2srgb(linearRGB.b));
}

float linearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;                                 // Back to NDC, [0, 1] -> [-1, 1]
    return (2.0 * near * far) / (far + near - z * (far - near)); // Linearize z, [-1, 1] -> [near, far]
}

float depth2RayLength(float z)
{
    vec2 _uv = 2.f * uv - 1.f;
    float x = _uv[0] * xMax;
    float y = _uv[1] * yMax;
    return sqrt(x * x + y * y + z * z);
}

// fast AABB intersection
vec2 intersectBox(vec3 orig, vec3 dir)
{
    const vec3 boxMin = -.5f * volumeScaling + volumeTranslate;
    const vec3 boxMax = +.5f * volumeScaling + volumeTranslate;
    const vec3 invDir = 1.0 / dir;
    const vec3 tmin_tmp = (boxMin - orig) * invDir;
    const vec3 tmax_tmp = (boxMax - orig) * invDir;
    const vec3 tmin = min(tmin_tmp, tmax_tmp);
    const vec3 tmax = max(tmin_tmp, tmax_tmp);
    const float tn = max(tmin.x, max(tmin.y, tmin.z));
    const float tf = min(tmax.x, min(tmax.y, tmax.z));
    return vec2(tn, tf);
}

// Pseudo-random number generator that approximtes U(0, 1)
// http://www.reedbeta.com/blog/quick-and-easy-gpu-random-numbers-in-d3d11/
float wangHash(int seed)
{
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return float(seed % 2147483647) / 2147483647.f;
}

float getErosionWeightQuntic(float density)
{
    return pow((1.f - density), 6);
}

float getErosionWeightCubic(float density)
{
    return (1.f - density) * (1.f - density) * (1.f - density);
}

// Henyey-Greenstein Phase Function
// inParam: float angle, float phaseParam (forwardScattering, backwardScattering) --> this is passed in hyperparam
// outParam: float phaseVal
float henyeyGreenstein(float cosTheta, float g)
{
    const float g2 = g * g;
    return (1 - g2) / (FOUR_PI * pow(1 + g2 - 2 * g * cosTheta, 1.5));
}

float phase(float cosTheta)
{
    const float blend = 0.5;
    const float hgBlend = henyeyGreenstein(cosTheta, phaseParams.x) * (1 - blend) + henyeyGreenstein(cosTheta, phaseParams.y) * (blend);
    return phaseParams.z + hgBlend * phaseParams.w;
}

float yFalloff(vec3 position)
{
    float ymin = -.5f * volumeScaling.y + volumeTranslate.y;
    float ymax = ymin + volumeScaling.y;
    float distY = min(Y_FALLOFF_DIST, min(position.y - ymin, ymax - position.y));
    return distY / Y_FALLOFF_DIST;
}

float xzFalloff(vec3 position)
{
    float xmin = -.5f * volumeScaling.x + volumeTranslate.x;
    float xmax = xmin + volumeScaling.x;
    float zmin = -.5f * volumeScaling.z + volumeTranslate.z;
    float zmax = zmin + volumeScaling.z;
    float distX = min(XZ_FALLOFF_DIST, min(position.x - xmin, xmax - position.x));
    float distZ = min(XZ_FALLOFF_DIST, min(position.z - zmin, zmax - position.z));
    return min(distX, distZ) / XZ_FALLOFF_DIST;
}

float sampleDensity(vec3 position)
{
    // Sample high-res shape textures

    const vec3 hiResT = .1f * hiResNoiseTranslate;
    const vec4 hiResS = .1f * hiResNoiseScaling;

    const mat4x3 hiResPosition = outerProduct(position, hiResS) + outerProduct(hiResT, vec4(1.f));

    const vec4 hiResNoise = vec4(texture(volumeHighRes, hiResPosition[0]).r, texture(volumeHighRes, hiResPosition[1]).g, texture(volumeHighRes, hiResPosition[2]).b, texture(volumeHighRes, hiResPosition[3]).a);

    //    const vec3 hiResPosition = position * .1f * hiResNoiseScaling[0] * .1f + hiResNoiseTranslate; // TODO: hiResNoiseScaling change to 4 channel scaling
    //    const vec4 hiResNoise = texture(volumeHighRes, hiResPosition);
    float hiResDensity = dot(hiResNoise, normalizeL1(hiResChannelWeights));
    if (invertDensity)
        hiResDensity = 1.f - hiResDensity;

    // Reduce density at the bottom of the cloud to create crisp shape
    float falloff = yFalloff(position) * xzFalloff(position);
    hiResDensity *= falloff;

    // Control the cover of clouds by offsetting density
    float hiResDensityWithOffset = hiResDensity + hiResDensityOffset;

    // Skip adding details if there is no cloud to begin with
    if (hiResDensityWithOffset <= 0.f)
        return 0.f;

    // Sample low-res detail textures
    const vec3 loResPosition = position * loResNoiseScaling * .1f + loResNoiseTranslate;
    const vec4 loResNoise = texture(volumeLowRes, loResPosition);
    float loResDensity = dot(loResNoise, normalizeL1(loResChannelWeights));
    loResDensity = 1.f - loResDensity; // invert the low-res density by default

    // Detail erosion: subtract low-res detail from hi-res noise, weighted as such that
    // the erosion is more pronounced near the boudary of the cloud (low hiResDensity)
    //    const float erosionWeight = getErosionWeightCubic(hiResDensity);
    const float erosionWeight = getErosionWeightQuntic(hiResDensity);

    const float density = hiResDensityWithOffset - erosionWeight * loResDensityWeight * loResDensity;
    return max(density * densityMult * 5.f, 0.f);
}

// One-bounce raymarch to get light transmittance
float computeLightTransmittance(vec3 rayOrig, vec3 rayDir)
{
    const int numStepsRecursive = numSteps / 8;
    const vec2 tHit = intersectBox(rayOrig, rayDir);
    const float tFar = max(0.f, tHit.y);
    const float dt = tFar / numStepsRecursive;
    const vec3 ds = rayDir * dt;

    float tau = 0.f; // log transmittance
    vec3 pointWorld = rayOrig;
    for (float t = 0.f; t < tFar; t += dt)
    {
        const float density = sampleDensity(pointWorld);
        tau -= density;
        pointWorld += ds;
    }
    tau *= (cloudLightAbsorptionMult * dt); // delay multiplication to save compute and avoid precision issues
    float lightTransmittance = exp(tau);

    // ambient hack to make clouds less dark
    return minLightTransmittance + lightTransmittance * (1.f - minLightTransmittance);
}

//------------Skycolor-------------------------------------------------------------------
// Simulates an atmosphere
// return the distance traveled inside the atmosphere
float raySphere(vec3 sphereCenter, float sphereRadius, vec3 rayOrigin, vec3 rayDir)
{
    vec3 offset = rayOrigin - sphereCenter;
    float a = 1;
    float b = 2 * dot(offset, rayDir);
    float c = dot(offset, offset) - sphereRadius * sphereRadius;
    float d = b * b - 4 * a * c;

    if (d >= 0)
    {
        float s = sqrt(d);
        float dstNear = max(0, (-b - s) / (2 * a));
        float dstFar = (-b + s) / (2 * a);
        if (dstFar >= 0)
        {
            return dstFar - dstNear;
        }
    }
    return 0.0;
}

// Used for sky scattering, simulates the particle density in the atmosphere
float densityAtPoint(vec3 pointPosWorld, vec3 planetCenter, float planetRadius, float atmosRadius)
{
    float densityFalloff = 4.0;
    float height = length(pointPosWorld - planetCenter) - planetRadius;
    float height01 = height / (atmosRadius - planetRadius);
    float density = exp(-height01 * densityFalloff) * (1 - height01);
    return density;
}

// optical depth: average density along the ray, determined by the raylength (from point to the sun, within the atmosphere)
float opticalDepth(vec3 rayOrig, vec3 rayDir, float rayLength, vec3 planetCenter, float planetRadius, float atmosRadius)
{
    int numOpticalPoints = 10;
    vec3 densitySamplePoint = rayOrig;
    float stepSize = rayLength / (numOpticalPoints - 1);
    float opticalDepth = 0;
    for (int i = 0; i < numOpticalPoints; i++)
    {
        float localDensity = densityAtPoint(densitySamplePoint, planetCenter, planetRadius, atmosRadius);
        opticalDepth += localDensity * stepSize;
        densitySamplePoint += rayDir * stepSize;
    }
    return opticalDepth;
}

// query sun color texture based on height of the sun
vec3 getSunColor(float longitudeRadians)
{
    float timeOfDay = abs(longitudeRadians) / HALF_PI; // 0: noon, 1: dusk/dawn
    return texture(sunGradient, timeOfDay).rgb;
}

vec4 getNightColor(float longitudeRadians)
{
    float timeOfDay = abs(longitudeRadians) / HALF_PI; // 0: noon, 1: dusk/dawn
    vec2 newUv = vec2(uv[0], 1.0 - uv[1]);
    vec4 origColor = texture(nightColor, uv);
    float gray = 0.2989 * origColor[0] + 0.5870 * origColor[1] + 0.1140 * origColor[2];
    float newR = -gray * timeOfDay + origColor[0] * (1 + timeOfDay);
    float newG = -gray * timeOfDay + origColor[1] * (1 + timeOfDay);
    float newB = -gray * timeOfDay + origColor[2] * (1 + timeOfDay);

    return vec4(newR, newG, newB, origColor[2]) * 0.3;
    //    return texture(nightColor, uv);
}

void main()
{
    /* ---------------------- solid geometry ----------------------  */
    const float zSolid = linearizeDepth(texture(solidDepth, uv).r);
    const float tHitSolid = depth2RayLength(zSolid);
    const vec4 colorSolid = texture(solidColor, uv);

    /* ---------------------------- ray ---------------------------- */
    const vec3 rayDirWorld = normalize(rayDirWorldspace);
    vec2 tHit = intersectBox(rayOrigWorld, rayDirWorld);
    tHit.x = max(tHit.x, 0.f);       // keep the near intersection in front of the camera
    tHit.y = min(tHit.y, tHitSolid); // keep far intersection in front of solid geometry
    vec3 pointWorld = rayOrigWorld + tHit.x * rayDirWorld;

    /* -------------------------- light ---------------------------- */
    const float sunLatitudeRadians = radians(testLight.latitude);
    const float sunLongitudeRadians = radians(testLight.longitude);
    const vec3 sunDirSpherical = dirSph2Cart(sunLatitudeRadians, sunLongitudeRadians);
    const vec3 sunPos = SUN_RADIUS * sunDirSpherical;
    const vec3 dirLight = normalize(sunPos - pointWorld); // use actual sun location for more epic sunset
                                                          //    const vec3 dirLight = dirSph2Cart(sunLatitudeRadians, sunLongitudeRadians);  // towards the light

    const float cosRayLightAngle = dot(rayDirWorld, dirLight);
    const float phaseVal = phase(cosRayLightAngle); // directional light only for now
    const vec3 sunColor = getSunColor(sunLongitudeRadians);

    /* ----------------------------- sky -------------------------- */
    vec3 inScatteredLight = vec3(0.0, 0.0, 0.0);
    float scatteringStrength = 0.09;
    vec3 wavelengths = vec3(700, 530, 440);
    float scatterR = pow(400 / wavelengths[0], 4) * scatteringStrength;
    float scatterG = pow(400 / wavelengths[1], 4) * scatteringStrength;
    float scatterB = pow(400 / wavelengths[2], 4) * scatteringStrength;
    vec3 scatteringCoeff = vec3(scatterR, scatterG, scatterB);

    float viewRayOpticalDepth = 0.0;
    int numInScatteringPoints = 10;

    // Create atmosphere
    float atmosRadius = 1050.0;
    float planetRadius = 1000.0;
    vec3 planetCenter = vec3(0.0, -planetRadius, 0.0);

    // ================================================================== //

    // Volume rendering with adaptive step sizes
    vec3 cloudColor = vec3(0.f);
    float transmittance = 1.f;
    float lightEnergy = 0.f;
    if (tHit.x < tHit.y)
    { // hit box
        // Starting from the near intersection, march the ray forward and sample
        float dstTravelled = 0;
        float totalDst = (tHit.y - tHit.x);
        float curFineStepSize = min(STEPSIZE_FINE, totalDst / MIN_NUM_FINE_STEPS);
        float curCoarseStepSize = curFineStepSize * COARSE_STEPSIZE_MULTIPLIER;
        int curThreshold = MAX_NUM_MISSED_STEPS;
        float dt = curCoarseStepSize;

        // Optionally apply random offset on ray start to minimize color banding
        const int seed = int(gl_FragCoord.y + 3000 * gl_FragCoord.x);
        const float eps = wangHash(seed);
        const float offset = eps * curFineStepSize; // max offset is one coarse step
        pointWorld += offset * rayDirWorld;
        dstTravelled += offset;

        while (dstTravelled < totalDst)
        {
            // sample density and evaluate vol rendering equation
            float density = sampleDensity(pointWorld);
            if (density > 0.f)
            {
                float lightTransmittance = computeLightTransmittance(pointWorld, dirLight);
                lightEnergy += density * transmittance * lightTransmittance * dt;
                transmittance *= (1 - density * cloudLightAbsorptionMult * dt); // Taylor approx for exp(-density * cloudLightAbsorptionMult * dt)
                if (transmittance < EARLY_STOP_THRESHOLD)
                    break;
                curThreshold = MAX_NUM_MISSED_STEPS; // hit cloud, reset countdown
            }
            else
            {
                curThreshold -= 1;
            }

            // advance ray sample point
            dstTravelled += dt;
            pointWorld += rayDirWorld * dt;

            // switch to coarse if we missed too many steps, otherwise use fine
            dt = curThreshold <= 0 ? curCoarseStepSize : curFineStepSize;
        }

        // TODO: adjust sunColor at night
        lightEnergy *= phaseVal;
        cloudColor = lightEnergy * sunColor;
    }



    //----------------------------skycolor related-------------------------------
    // Compute color of the sky (background)
    float scaler = 70.0;
    pointWorld = rayOrigWorld;
    float rayLength = raySphere(planetCenter, atmosRadius, pointWorld, normalize(rayDirWorld));
    float stepSize = rayLength / (numInScatteringPoints - 1);

    for (int i = 0; i < numInScatteringPoints; i++)
    {
        float localDensity = densityAtPoint(pointWorld, planetCenter, planetRadius, atmosRadius);
        float sunRayLength = raySphere(planetCenter, atmosRadius, pointWorld, sunDirSpherical);

        float sunRayOpticalDepth = opticalDepth(pointWorld, sunDirSpherical, sunRayLength, planetCenter, planetRadius, atmosRadius);

        viewRayOpticalDepth = opticalDepth(pointWorld, -rayDirWorld, stepSize * i, planetCenter, planetRadius, atmosRadius);
        vec3 transSky = exp(-(sunRayOpticalDepth + viewRayOpticalDepth) * scatteringCoeff);
        inScatteredLight += localDensity * transSky * scatteringCoeff * stepSize;
        pointWorld += rayDirWorld * stepSize;
    }

    vec3 backgroundColor;
    float sunIntensity;
    float timeOfDay = abs(sunLongitudeRadians) / HALF_PI; // 0: noon, 1: dusk/dawn

    if (raySphere(planetCenter, planetRadius, rayOrigWorld, rayDirWorld) > 0.0)
    {
        // if below the horizon, set bg to black and zero sun intensity
        backgroundColor = vec3(0.f);
        sunIntensity = 0;
    }
    else
    {
        // otherwise, composite sky and the sun normally
        vec3 originalColor = vec3(0.0, 0.0, 0.0);
        originalColor = vec3(getNightColor(sunLongitudeRadians));
        float originalColTrans = exp(-viewRayOpticalDepth);
        backgroundColor = originalColor * originalColTrans + inScatteredLight;

        float coeff;
        float lower_threshold = 1.0;
        float hi = 1.5;
        if (timeOfDay < lower_threshold)
        {
            coeff = originalColTrans;
            backgroundColor = vec3(0.0) * coeff + inScatteredLight;
        }
        else
        {
            coeff = 1.0 / (hi - lower_threshold) * (1 - originalColTrans) * (timeOfDay - hi) + 1.0;
            backgroundColor = originalColor * coeff + inScatteredLight;
        }

        sunIntensity = henyeyGreenstein(dot(rayDirWorld, sunDirSpherical), .9995) * transmittance;
        sunIntensity = min(sunIntensity, MAX_SUN_INTENSITY);
    }
    if (texture(solidDepth, uv).r < 0.5)
    { // hit solid
        backgroundColor = colorSolid.rgb;
        sunIntensity = 0;
    }

    if (timeOfDay > 0.999)
    {
        float alpha = 1.0 / 0.2 * (timeOfDay - 0.999);
        cloudColor = (1 - alpha) * cloudColor;
    }

glFragColor = vec4(backgroundColor, 1.f);

    vec3 cloudOnBackground = min(cloudColor + transmittance * backgroundColor, 1.f);

    // blend sun color in cloud+bg
    vec3 compositeColor = cloudOnBackground * max(1 - sunIntensity, 0.f) + sunColor * sunIntensity;

    if (gammaCorrect)
        compositeColor = gammaCorrection(compositeColor);
    // glFragColor = vec4(compositeColor, 1.f);

    // DEBUG
    //    glFragColor = vec4(testLight.color, 1.f);
    //    float sigma = sampleDensity(positionWorld);
    //   vec3 position = positionWorld * hiResNoiseScaling * .1f + hiResNoiseTranslate * .1f;
    // show noise channels as BW images
    //    float sigma = sampleDensity(positionWorld);
    //    float sigma = texture(volumeHighRes, position).g;
    //    float sigma = texture(volumeHighRes, position).b;
    //    float sigma = texture(volumeHighRes, position).a;
    //    float sigma = texture(volumeLowRes, position).r;
    //    float sigma = texture(volumeLowRes, position).g;
    //    float sigma = texture(volumeLowRes, position).b;
    //    float sigma = texture(volumeLowRes, position).a;
    //    glFragColor = vec4(sigma);

    // show noise channels as is
    //    glFragColor = vec4(1.f);
    //    glFragColor.r = texture(nightColor, uv).r;
    //    glFragColor.g = texture(nightColor, uv).g;
    //    glFragColor.b = texture(nightColor, uv).b;
    //    glFragColor.a = texture(nightColor, uv).a;

    // final processing
    //    if (invertDensity)
    //        glFragColor = 1.f - glFragColor;
    //    glFragColor *= densityMult;
    //    glFragColor.a = 1.f;
}
