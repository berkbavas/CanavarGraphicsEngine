#include "Haze.h"

#include "Canavar/Engine/Core/Shader.h"

Canavar::Engine::Haze::Haze()
{
    SetNodeName("Haze");
}

void Canavar::Engine::Haze::SetUniforms(Shader *pShader) const
{
    pShader->Bind();
    pShader->SetUniform("uHaze.Enabled", mEnabled);
    pShader->SetUniform("uHaze.Color", mColor);
    pShader->SetUniform("uHaze.Density", mDensity);
    pShader->SetUniform("uHaze.Gradient", mGradient);
    pShader->Unbind();
}
