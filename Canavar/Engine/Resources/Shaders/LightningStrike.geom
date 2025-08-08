#version 450

layout(lines) in;
layout(line_strip, max_vertices = 32) out;

in vec3 gsWorldPosition[];
in float gsForkLevel[];

out vec3 outWorldPosition;
out float outForkLevel;

uniform float uElapsedTime = 0;
uniform float uJitterDisplacement;
uniform float uForkLength;
uniform int uMode; // 0-> Jitter, 1-> Fork

// ########## The code below is taken from https://stackoverflow.com/a/17479300 ##########
// A single iteration of Bob Jenkins' One-At-A-Time hashing algorithm.
uint hash(uint x)
{
    x += (x << 10u);
    x ^= (x >> 6u);
    x += (x << 3u);
    x ^= (x >> 11u);
    x += (x << 15u);
    return x;
}

// Compound versions of the hashing algorithm I whipped together.
uint hash(uvec2 v)
{
    return hash(v.x ^ hash(v.y));
}
uint hash(uvec3 v)
{
    return hash(v.x ^ hash(v.y) ^ hash(v.z));
}
uint hash(uvec4 v)
{
    return hash(v.x ^ hash(v.y) ^ hash(v.z) ^ hash(v.w));
}

// Construct a float with half-open range [0:1] using low 23 bits.
// All zeroes yields 0.0, all ones yields the next smallest representable value below 1.0.
float floatConstruct(uint m)
{
    const uint ieeeMantissa = 0x007FFFFFu; // binary32 mantissa bitmask
    const uint ieeeOne = 0x3F800000u;      // 1.0 in IEEE binary32

    m &= ieeeMantissa; // Keep only mantissa bits (fractional part)
    m |= ieeeOne;      // Add fractional part to 1.0

    float f = uintBitsToFloat(m); // Range [1:2]
    return f - 1.0;               // Range [0:1]
}

// Pseudo-random value in half-open range [0:1].
float random(float x)
{
    return floatConstruct(hash(floatBitsToUint(uElapsedTime + x)));
}
float random(vec2 v)
{
    return floatConstruct(hash(floatBitsToUint(v)));
}
float random(vec3 v)
{
    return floatConstruct(hash(floatBitsToUint(v)));
}
float random(vec4 v)
{
    return floatConstruct(hash(floatBitsToUint(v)));
}
// ########## The code above is taken from https://stackoverflow.com/a/17479300 ##########

// Generates a random unit vector orthogonal to p1 - p0.
vec3 generateJumpVector(vec3 p0, vec3 p1)
{
    vec3 dir = normalize(p1 - p0);

    float jx = 1 - 2 * random(dir.x + 1.0f);
    float jy = 1 - 2 * random(dir.y + 2.0f);
    float jz = 1 - 2 * random(dir.z + 3.0f);

    return cross(dir, normalize(vec3(jx, jy, jz)));
}

void main()
{
    float forkLevel = gsForkLevel[1];
    vec3 p0 = gsWorldPosition[0];
    vec3 p1 = gsWorldPosition[1];
    vec3 middlePoint = 0.5f * (p0 + p1);
    vec3 jitterJumpVector = generateJumpVector(p0, p1);
    vec3 newMiddlePoint = middlePoint + uJitterDisplacement * jitterJumpVector;

    // Jittering
    outWorldPosition = p0;
    outForkLevel = forkLevel;
    EmitVertex();

    outWorldPosition = newMiddlePoint;
    outForkLevel = forkLevel;
    EmitVertex();
    EndPrimitive();

    outWorldPosition = newMiddlePoint;
    outForkLevel = forkLevel;
    EmitVertex();

    outWorldPosition = p1;
    outForkLevel = forkLevel;
    EmitVertex();
    EndPrimitive();

    if (uMode == 1) // Fork
    {
        vec3 newDirection = normalize(newMiddlePoint - p0);
        vec3 prevDir = normalize(p1 - p0);
        float s = random(10.0f);
        vec3 forkDirection = normalize(s * prevDir + (1 - s) * newDirection);
        vec3 forkEndPoint = newMiddlePoint + uForkLength * forkDirection;

        outWorldPosition = newMiddlePoint;
        outForkLevel = forkLevel + 1.0f;
        EmitVertex();

        outWorldPosition = forkEndPoint;
        outForkLevel = forkLevel + 1.0f;
        EmitVertex();
        EndPrimitive();
    }
}