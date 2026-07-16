#include "CinematicEffect.h"

#include "Canavar/Engine/Core/Framebuffer.h"

#include <QDateTime>

Canavar::Engine::CinematicEffect::CinematicEffect()
{
    mShader = std::make_unique<Shader>("Cinematic Shader");
    mShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Quad.vert");
    mShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Cinematic.frag");
    mShader->Initialize();

    mQuad = std::make_unique<Quad>();

    UpdateTime();
}

void Canavar::Engine::CinematicEffect::ApplyEffect(Framebuffer *pInput, Framebuffer *pOutput)
{
    UpdateTime();

    pOutput->Bind();
    glViewport(0, 0, pOutput->GetWidth(), pOutput->GetHeight());
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    mShader->Bind();
    mShader->SetSampler("uSceneTexture", 0, pInput->GetTexture());
    mShader->SetUniform("uTime", mTime);
    mShader->SetUniform("uGrainStrength", mGrainStrength);
    mShader->SetUniform("uResolution", QVector2D(pOutput->GetWidth(), pOutput->GetHeight()));
    mShader->SetUniform("uVignetteRadius", mVignetteRadius);
    mShader->SetUniform("uVignetteSoftness", mVignetteSoftness);
    mQuad->Render();
    mShader->Unbind();
    pOutput->Unbind();
}

void Canavar::Engine::CinematicEffect::UpdateTime()
{
    const auto CurrentTime = QDateTime::currentMSecsSinceEpoch();
    mTime += (CurrentTime - mLastTime) / 1000.0f;
    mLastTime = CurrentTime;
}
