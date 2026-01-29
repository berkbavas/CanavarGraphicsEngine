#version 450 core

uniform sampler2D uSceneTexture;
uniform vec2 uResolution;
uniform float uFxaaSpanMax;
uniform float uFxaaReduceMin;
uniform float uFxaaReduceMul;

in vec2 fsTextureCoords;

out vec4 OutFragColor;

// FXAA implementation based on NVIDIA's FXAA 3.11
float Luminance(vec3 color)
{
    return dot(color, vec3(0.299, 0.587, 0.114));
}

void main()
{
    vec2 texelSize = 1.0 / uResolution;
    
    // Sample the center and neighboring pixels
    vec3 rgbNW = texture(uSceneTexture, fsTextureCoords + vec2(-1.0, -1.0) * texelSize).rgb;
    vec3 rgbNE = texture(uSceneTexture, fsTextureCoords + vec2( 1.0, -1.0) * texelSize).rgb;
    vec3 rgbSW = texture(uSceneTexture, fsTextureCoords + vec2(-1.0,  1.0) * texelSize).rgb;
    vec3 rgbSE = texture(uSceneTexture, fsTextureCoords + vec2( 1.0,  1.0) * texelSize).rgb;
    vec3 rgbM  = texture(uSceneTexture, fsTextureCoords).rgb;
    
    // Calculate luminance
    float lumaNW = Luminance(rgbNW);
    float lumaNE = Luminance(rgbNE);
    float lumaSW = Luminance(rgbSW);
    float lumaSE = Luminance(rgbSE);
    float lumaM  = Luminance(rgbM);
    
    // Calculate contrast range
    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
    
    // Calculate edge direction
    vec2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));
    
    float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * uFxaaReduceMul), uFxaaReduceMin);
    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
    
    dir = min(vec2(uFxaaSpanMax), max(vec2(-uFxaaSpanMax), dir * rcpDirMin)) * texelSize;
    
    // Perform blur along edge direction
    vec3 rgbA = 0.5 * (
        texture(uSceneTexture, fsTextureCoords + dir * (1.0 / 3.0 - 0.5)).rgb +
        texture(uSceneTexture, fsTextureCoords + dir * (2.0 / 3.0 - 0.5)).rgb);
    
    vec3 rgbB = rgbA * 0.5 + 0.25 * (
        texture(uSceneTexture, fsTextureCoords + dir * -0.5).rgb +
        texture(uSceneTexture, fsTextureCoords + dir *  0.5).rgb);
    
    float lumaB = Luminance(rgbB);
    
    // Choose result based on local contrast
    if ((lumaB < lumaMin) || (lumaB > lumaMax))
    {
        OutFragColor = vec4(rgbA, 1.0);
    }
    else
    {
        OutFragColor = vec4(rgbB, 1.0);
    }
}
