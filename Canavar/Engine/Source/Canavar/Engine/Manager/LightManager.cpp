#include "LightManager.h"

#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/Light/DirectionalLight.h"
#include "Canavar/Engine/Light/PointLight.h"

QVector<Canavar::Engine::PointLight *> Canavar::Engine::LightManager::GetPointLightsAround(QVector3D TargetPosition, float Radius) const
{
    QVector<PointLight *> Result;

    // Filter point lights based on distance to the target position
    for (const auto &pLight : mPointLights)
    {
        // Skip if disabled
        if (!pLight->GetEnabled())
        {
            continue;
        }

        const auto Distance = pLight->GetPosition().distanceToPoint(TargetPosition);

        if (Distance <= Radius)
        {
            Result.push_back(pLight);
        }
    }

    // Sort based on distance to the target position
    std::sort(Result.begin(), Result.end(), [TargetPosition](PointLight *pFirst, PointLight *pSecond) {
        const auto FirstDistance = pFirst->GetPosition().distanceToPoint(TargetPosition);
        const auto SecondDistance = pSecond->GetPosition().distanceToPoint(TargetPosition);
        return FirstDistance < SecondDistance;
    });

    return Result;
}

const QList<Canavar::Engine::PointLight *> &Canavar::Engine::LightManager::GetPointLights() const
{
    return mPointLights;
}

const QList<Canavar::Engine::DirectionalLight *> &Canavar::Engine::LightManager::GetDirectionalLights() const
{
    return mDirectionalLights;
}

QList<Canavar::Engine::DirectionalLight *> Canavar::Engine::LightManager::GetEnabledDirectionalLights() const
{
    QList<DirectionalLight *> EnabledDirectionalLights;

    for (auto *pDirectionalLight : mDirectionalLights)
    {
        if (pDirectionalLight->GetEnabled())
        {
            EnabledDirectionalLights.push_back(pDirectionalLight);
        }
    }

    return EnabledDirectionalLights;
}

void Canavar::Engine::LightManager::AddLight(Light *pLight)
{
    if (auto *pDirectionalLight = dynamic_cast<DirectionalLight *>(pLight))
    {
        mDirectionalLights.push_back(pDirectionalLight);
    }
    else if (auto *pPointLight = dynamic_cast<PointLight *>(pLight))
    {
        mPointLights.push_back(pPointLight);
    }
}

void Canavar::Engine::LightManager::RemoveLight(Light *pLight)
{
    if (auto *pDirectionalLight = dynamic_cast<DirectionalLight *>(pLight))
    {
        mDirectionalLights.removeAll(pDirectionalLight);
    }
    else if (auto *pPointLight = dynamic_cast<PointLight *>(pLight))
    {
        mPointLights.removeAll(pPointLight);
    }
}

void Canavar::Engine::LightManager::SetDirectionalLightsUniforms(Shader *pShader) const
{
    const auto EnabledDirectionalLights = GetEnabledDirectionalLights();

    const auto NumDirectionalLights = std::min(static_cast<int>(EnabledDirectionalLights.size()), MAX_DIRECTIONAL_LIGHTS);

    pShader->Bind();
    pShader->SetUniform("uNumDirectionalLights", NumDirectionalLights);

    for (int i = 0; i < NumDirectionalLights; ++i)
    {
        const auto *pDirectionalLight = EnabledDirectionalLights[i];

        pShader->SetUniform(QString("uDirectionalLights[%1].Color").arg(i), pDirectionalLight->GetColor());
        pShader->SetUniform(QString("uDirectionalLights[%1].Direction").arg(i), pDirectionalLight->GetDirection().normalized());
        pShader->SetUniform(QString("uDirectionalLights[%1].Ambient").arg(i), pDirectionalLight->GetAmbient());
        pShader->SetUniform(QString("uDirectionalLights[%1].Diffuse").arg(i), pDirectionalLight->GetDiffuse());
        pShader->SetUniform(QString("uDirectionalLights[%1].Specular").arg(i), pDirectionalLight->GetSpecular());
        pShader->SetUniform(QString("uDirectionalLights[%1].Radiance").arg(i), pDirectionalLight->GetRadiance());
    }

    pShader->Unbind();
}

void Canavar::Engine::LightManager::SetPointLightsUniforms(Shader *pShader, const QVector3D &TargetPosition, float Radius) const
{
    const auto PointLightsAround = GetPointLightsAround(TargetPosition, Radius);
    const auto NumPointLights = std::min(static_cast<int>(PointLightsAround.size()), MAX_POINT_LIGHTS);

    pShader->Bind();
    pShader->SetUniform("uNumPointLights", NumPointLights);

    for (int i = 0; i < NumPointLights; ++i)
    {
        const auto *pPointLight = PointLightsAround[i];

        pShader->SetUniform(QString("uPointLights[%1].Color").arg(i), pPointLight->GetColor());
        pShader->SetUniform(QString("uPointLights[%1].Position").arg(i), pPointLight->GetPosition());
        pShader->SetUniform(QString("uPointLights[%1].Ambient").arg(i), pPointLight->GetAmbient());
        pShader->SetUniform(QString("uPointLights[%1].Diffuse").arg(i), pPointLight->GetDiffuse());
        pShader->SetUniform(QString("uPointLights[%1].Specular").arg(i), pPointLight->GetSpecular());
        pShader->SetUniform(QString("uPointLights[%1].Constant").arg(i), pPointLight->GetConstant());
        pShader->SetUniform(QString("uPointLights[%1].Linear").arg(i), pPointLight->GetLinear());
        pShader->SetUniform(QString("uPointLights[%1].Quadratic").arg(i), pPointLight->GetQuadratic());
    }

    pShader->Unbind();
}
