#include "Sky.h"

#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/Node/Object/Camera/Camera.h"
#include "Canavar/Engine/Node/Object/Light/DirectionalLight.h"

#include <QFile>
#include <QVector3D>
#include <QtMath>

void Canavar::Engine::Sky::Initialize()
{
    initializeOpenGLFunctions();

    mQuad = new Quad;
}

void Canavar::Engine::Sky::Render(Shader* pShader, DirectionalLight* pSun, Camera* pCamera)
{
    if (mEnabled == false)
    {
        return;
    }

    glDisable(GL_DEPTH_TEST);
    pShader->Bind();
    pShader->SetUniformValue("uInvProjectionMatrix", pCamera->GetProjectionMatrix().inverted());
    pShader->SetUniformValue("uInvViewMatrix", pCamera->GetRotationMatrix().inverted());
    pShader->SetUniformValue("uCameraPos", pCamera->GetWorldPosition());
    pShader->SetUniformValue("uSunPos", -pSun->GetDirection().normalized());
    pShader->SetUniformValue("uHorizonOffset", mHorizonOffset);
    pShader->SetUniformValue("uSunIntensity", mSunIntensity);
    pShader->SetUniformValue("uPlanetRadius", mPlanetRadius);
    pShader->SetUniformValue("uAtmosphereRadius", mAtmosphereRadius);
    pShader->SetUniformValue("uBetaRayleigh", mBetaRayleigh);
    pShader->SetUniformValue("uBetaMie", mBetaMie);
    pShader->SetUniformValue("uScaleHeightR", mScaleHeightR);
    pShader->SetUniformValue("uScaleHeightM", mScaleHeightM);
    pShader->SetUniformValue("uMieG", mMieG);
    mQuad->Render();
    pShader->Release();
    glEnable(GL_DEPTH_TEST);
}
