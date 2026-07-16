#include "LensDistortionEffect.h"

#include "Canavar/Engine/Core/Framebuffer.h"

Canavar::Engine::LensDistortionEffect::LensDistortionEffect()
{
    mShader = std::make_unique<Shader>("Lens Distortion Shader");
    mShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Quad.vert");
    mShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/LensDistortion.frag");
    mShader->Initialize();

    mQuad = std::make_unique<Quad>();
}

void Canavar::Engine::LensDistortionEffect::ApplyEffect(Framebuffer* pInput, Framebuffer* pOutput)
{
    pOutput->Bind();
    glViewport(0, 0, pOutput->GetWidth(), pOutput->GetHeight());
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    mShader->Bind();
    mShader->SetSampler("uSceneTexture", 0, pInput->GetTexture());
    mShader->SetUniform("uBarrel", mBarrel);
    mShader->SetUniform("uZoom", mZoom);
    mQuad->Render();
    mShader->Unbind();
    pOutput->Unbind();
}
