#include "LightManager.h"

#include "Canavar/Engine/Util/Logger.h"

Canavar::Engine::LightManager::LightManager(QObject* parent)
    : Manager(parent)
{}

void Canavar::Engine::LightManager::Initialize() {}

QVector<Canavar::Engine::PointLightPtr> Canavar::Engine::LightManager::GetPointLightsAround(QVector3D targetPosition, float radius)
{
    QVector<PointLightPtr> result;

    for (const auto& pLight : mPointLights)
    {
        float distance = pLight->GetWorldPosition().distanceToPoint(targetPosition);

        if (distance <= radius)
        {
            result.push_back(pLight);
        }
    }

    return result;
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
