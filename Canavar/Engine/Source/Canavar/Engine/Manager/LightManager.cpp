#include "LightManager.h"

Canavar::Engine::LightManager::LightManager(QObject* parent)
    : Manager(parent)
{}

void Canavar::Engine::LightManager::Initialize()
{
    mSun = std::make_shared<DirectionalLight>();
    mSun->SetDirection(QVector3D(-1.0f, 0.75f, -1.0f).normalized());
    AddLight(mSun);
}

std::vector<Canavar::Engine::PointLightPtr> Canavar::Engine::LightManager::GetPointLightsAround(QVector3D targetPosition, float radius)
{
    std::vector<PointLightPtr> result;

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
    // TODO: Log

    if (DirectionalLightPtr pDirectionalLight = std::dynamic_pointer_cast<DirectionalLight>(pLight))
    {
        mDirectionalLights.push_back(pDirectionalLight);
    }
    else if (PointLightPtr pPointLight = std::dynamic_pointer_cast<PointLight>(pLight))
    {
        mPointLights.emplace(pPointLight);
    }
}

void Canavar::Engine::LightManager::RemoveLight(LightPtr pLight)
{
    // TODO: Log

    if (DirectionalLightPtr pDirectionalLight = std::dynamic_pointer_cast<DirectionalLight>(pLight))
    {
        mDirectionalLights.removeAll(pDirectionalLight);
    }
    else if (PointLightPtr pPointLight = std::dynamic_pointer_cast<PointLight>(pLight))
    {
        mPointLights.erase(pPointLight);
    }
}

const QVector<Canavar::Engine::DirectionalLightPtr>& Canavar::Engine::LightManager::GetDirectionalLights() const
{
    return mDirectionalLights;
}
