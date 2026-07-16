#include "DepthOfFieldEffect.h"

#include "Canavar/Engine/Core/Framebuffer.h"

Canavar::Engine::DepthOfFieldEffect::DepthOfFieldEffect()
{
    mShader = std::make_unique<Shader>("Depth Of Field Shader");
    mShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Quad.vert");
    mShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/DepthOfField.frag");
    mShader->Initialize();

    mQuad = std::make_unique<Quad>();
}

void Canavar::Engine::DepthOfFieldEffect::ApplyEffect(Framebuffer* pInput, Framebuffer* pOutput)
{
    pOutput->Bind();
    glViewport(0, 0, pOutput->GetWidth(), pOutput->GetHeight());
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    mShader->Bind();
    mShader->SetSampler("uSceneTexture", 0, pInput->GetTexture());
    mShader->SetSampler("uWorldPositionTexture", 1, mWorldPositionTexture);
    mShader->SetUniform("uCameraPosition", mCameraPosition);
    mShader->SetUniform("uFocusDistance", mFocusDistance);
    mShader->SetUniform("uFocusRange", mFocusRange);
    mShader->SetUniform("uMaxBlurRadius", mMaxBlurRadius);
    mShader->SetUniform("uResolution", QVector2D(static_cast<float>(pInput->GetWidth()), static_cast<float>(pInput->GetHeight())));
    mQuad->Render();
    mShader->Unbind();

    pOutput->Unbind();
}
