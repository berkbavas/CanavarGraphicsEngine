#include "SharpenEffect.h"

#include "Canavar/Engine/Core/Framebuffer.h"

Canavar::Engine::SharpenEffect::SharpenEffect()
{
    mShader = std::make_unique<Shader>("Sharpen Shader");
    mShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Quad.vert");
    mShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Sharpen.frag");
    mShader->Initialize();

    mQuad = std::make_unique<Quad>();
}

void Canavar::Engine::SharpenEffect::ApplyEffect(Framebuffer* pInput, Framebuffer* pOutput)
{
    pOutput->Bind();
    glViewport(0, 0, pOutput->GetWidth(), pOutput->GetHeight());
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    mShader->Bind();
    mShader->SetSampler("uSceneTexture", 0, pInput->GetTexture());
    mShader->SetUniform("uTexelSize", QVector2D(1.0f / pInput->GetWidth(), 1.0f / pInput->GetHeight()));
    mShader->SetUniform("uStrength", mStrength);
    mQuad->Render();
    mShader->Unbind();
    pOutput->Unbind();
}
