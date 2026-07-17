#include "Sky.h"

#include "Canavar/Engine/Camera/PerspectiveCamera.h"
#include "Canavar/Engine/Light/DirectionalLight.h"

Canavar::Engine::Sky::Sky()
{
    initializeOpenGLFunctions();

    SetNodeName("Sky");

    mQuad = std::make_unique<Quad>();

    mSkyShader = std::make_unique<Shader>("Sky Shader");
    mSkyShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Sky.vert");
    mSkyShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Sky.frag");
    mSkyShader->Initialize();
}

void Canavar::Engine::Sky::Render(PerspectiveCamera *pActiveCamera, DirectionalLight *pSun)
{
    if (mEnabled == false)
    {
        return;
    }

    // Disable depth testing to ensure the sky is rendered behind all other objects
    glDisable(GL_DEPTH_TEST);

    // Bind the sky shader and set its uniforms
    mSkyShader->Bind();
    mSkyShader->SetUniform("uInvProjectionMatrix", pActiveCamera->GetProjectionMatrix().inverted());
    mSkyShader->SetUniform("uInvViewMatrix", pActiveCamera->GetRotationMatrix().inverted());
    mSkyShader->SetUniform("uCameraPos", pActiveCamera->GetPosition());
    mSkyShader->SetUniform("uSunPos", -pSun->GetDirection().normalized());
    mSkyShader->SetUniform("uHorizonOffset", mHorizonOffset);
    mSkyShader->SetUniform("uSunIntensity", mSunIntensity);
    mSkyShader->SetUniform("uPlanetRadius", mPlanetRadius);
    mSkyShader->SetUniform("uAtmosphereRadius", mAtmosphereRadius);
    mSkyShader->SetUniform("uBetaRayleigh", mBetaRayleigh);
    mSkyShader->SetUniform("uBetaMie", mBetaMie);
    mSkyShader->SetUniform("uScaleHeightR", mScaleHeightR);
    mSkyShader->SetUniform("uScaleHeightM", mScaleHeightM);
    mSkyShader->SetUniform("uMieG", mMieG);
    mQuad->Render();
    mSkyShader->Unbind();

    // Re-enable depth testing for subsequent rendering
    glEnable(GL_DEPTH_TEST);
}
