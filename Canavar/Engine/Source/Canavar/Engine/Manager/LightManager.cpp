#include "LightManager.h"

#include "Canavar/Engine/Light/DirectionalLight.h"
#include "Canavar/Engine/Light/PointLight.h"

QVector<Canavar::Engine::PointLight *> Canavar::Engine::LightManager::GetPointLightsAround(QVector3D TargetPosition, float Radius)
{
    QVector<PointLight *> Result;

    for (const auto &pLight : mPointLights)
    {
        float Distance = pLight->GetPosition().distanceToPoint(TargetPosition);

        if (Distance <= Radius)
        {
            Result.push_back(pLight);
        }
    }

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
