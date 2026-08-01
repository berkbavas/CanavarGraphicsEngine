#include "AerialPerspectiveEffect.h"

#include "Canavar/Engine/Core/Framebuffer.h"

Canavar::Engine::AerialPerspectiveEffect::AerialPerspectiveEffect()
{
    mShader = std::make_unique<Shader>("Aerial Perspective Shader");
    mShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Quad.vert");
    mShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/AerialPerspective.frag");
    mShader->Initialize();

    mQuad = std::make_unique<Quad>();
}

void Canavar::Engine::AerialPerspectiveEffect::ApplyEffect(Framebuffer* pInput, Framebuffer* pOutput)
{
    pOutput->Bind();
    glViewport(0, 0, pOutput->GetWidth(), pOutput->GetHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    mShader->Bind();
    mShader->SetSampler("uSceneTexture", 0, pInput->GetTexture());
    mShader->SetSampler("uWorldPosTexture", 1, mWorldPositionTexture);
    mShader->SetUniform("uCameraPos", mCameraPosition);
    mShader->SetUniform("uSunPos", mSunDirection);
    mShader->SetUniform("uSunIntensity", mSunIntensity);
    mShader->SetUniform("uPlanetRadius", mPlanetRadius);
    mShader->SetUniform("uAtmosphereRadius", mAtmosphereRadius);
    mShader->SetUniform("uBetaRayleigh", mBetaRayleigh);
    mShader->SetUniform("uBetaMie", mBetaMie);
    mShader->SetUniform("uScaleHeightR", mScaleHeightR);
    mShader->SetUniform("uScaleHeightM", mScaleHeightM);
    mShader->SetUniform("uMieG", mMieG);
    mShader->SetUniform("uDensity", mDensity);
    mQuad->Render();
    mShader->Unbind();
    pOutput->Unbind();
}
