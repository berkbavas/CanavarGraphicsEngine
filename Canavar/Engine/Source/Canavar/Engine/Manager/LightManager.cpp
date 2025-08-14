#include "LightManager.h"

#include "Canavar/Engine/Util/Logger.h"

QVector<Canavar::Engine::PointLight*> Canavar::Engine::LightManager::GetPointLightsAround(QVector3D TargetPosition, float Radius)
{
    QVector<PointLight*> Result;

    for (const auto& pLight : mPointLights)
    {
        float Distance = pLight->GetWorldPosition().distanceToPoint(TargetPosition);

        if (Distance <= Radius)
        {
            Result.push_back(pLight.get());
        }
    }

    return Result;
}

void Canavar::Engine::LightManager::AddLight(LightPtr pLight)
{
    LOG_DEBUG("LightManager::AddLight: I will add this light to my list: {}", PRINT_ADDRESS(pLight.get()));

    if (DirectionalLightPtr pDirectionalLight = std::dynamic_pointer_cast<DirectionalLight>(pLight))
    {
        mDirectionalLights.push_back(pDirectionalLight);
        LOG_DEBUG("LightManager::AddLight: I have added a DirectionalLight.");
    }
    else if (PointLightPtr pPointLight = std::dynamic_pointer_cast<PointLight>(pLight))
    {
        mPointLights.push_back(pPointLight);
        LOG_DEBUG("LightManager::AddLight: I have added a PointLight.");
    }
}

void Canavar::Engine::LightManager::RemoveLight(LightPtr pLight)
{
    LOG_DEBUG("LightManager::RemoveLight: I will remove this light from my list: {}", PRINT_ADDRESS(pLight.get()));

    if (DirectionalLightPtr pDirectionalLight = std::dynamic_pointer_cast<DirectionalLight>(pLight))
    {
        int count = mDirectionalLights.removeAll(pDirectionalLight);
        LOG_DEBUG("LightManager::RemoveLight: I have removed {} DirectionalLight(s).", count);
    }
    else if (PointLightPtr pPointLight = std::dynamic_pointer_cast<PointLight>(pLight))
    {
        int count = mPointLights.removeAll(pPointLight);
        LOG_DEBUG("LightManager::RemoveLight: I have removed {} PointLight(s).", count);
    }
}

const QVector<Canavar::Engine::PointLightPtr>& Canavar::Engine::LightManager::GetPointLights() const
{
    return mPointLights;
}

const QVector<Canavar::Engine::DirectionalLightPtr>& Canavar::Engine::LightManager::GetDirectionalLights() const
{
    return mDirectionalLights;
}
