#include "LightManager.h"

Canavar::Engine::LightManager::LightManager()
    : Manager()
{}

Canavar::Engine::LightManager* Canavar::Engine::LightManager::Instance()
{
    static LightManager instance;
    return &instance;
}

bool Canavar::Engine::LightManager::Init()
{
    return true;
}

void Canavar::Engine::LightManager::RemoveLight(Light* light)
{
    if (auto pointLight = dynamic_cast<PointLight*>(light))
        mPointLights.removeAll(pointLight);
}

void Canavar::Engine::LightManager::AddLight(Light* light)
{
    if (auto pointLight = dynamic_cast<PointLight*>(light))
        mPointLights << pointLight;
}

const QList<Canavar::Engine::PointLight*>& Canavar::Engine::LightManager::GetPointLights() const
{
    return mPointLights;
}