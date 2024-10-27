#include "NodeManager.h"

#include "LightManager.h"
#include "Util/Logger.h"
#include "Util/ModelImporter.h"

Canavar::Engine::NodeManager::NodeManager(QObject* parent) {}

void Canavar::Engine::NodeManager::Initialize()
{
    LOG_DEBUG("NodeManager::Initialize: Initializing...");

    mLightManager = mManagerProvider->GetLightManager();

    mScenes = ModelImporter::Import("Resources/Models", { "*.obj", "*.blend", "*.fbx", "*.glb", "*.gltf" });

    mSky = std::make_shared<Sky>();
    mTerrain = std::make_shared<Terrain>();
    mHaze = std::make_shared<Haze>();

    LOG_DEBUG("NodeManager::Initialize: Initialization is done.");
}

void Canavar::Engine::NodeManager::PostInitialize()
{
    mSky->Initialize();
    mTerrain->Initialize();

    for (const auto [name, pScene] : mScenes)
    {
        pScene->Initialize();
    }
}

void Canavar::Engine::NodeManager::AddNode(NodePtr pNode)
{
    // TODO: Log

    if (const auto pModel = std::dynamic_pointer_cast<Model>(pNode))
    {
        mModels.emplace(pModel);
    }
    else if (const auto pLight = std::dynamic_pointer_cast<Light>(pNode))
    {
        mLightManager->AddLight(pLight);
    }

    mNodes.emplace(pNode);
}

void Canavar::Engine::NodeManager::RemoveNode(NodePtr pNode)
{
    // TODO: Log

    if (const auto pModel = std::dynamic_pointer_cast<Model>(pNode))
    {
        mModels.erase(pModel);
    }
    else if (const auto pLight = std::dynamic_pointer_cast<Light>(pNode))
    {
        mLightManager->RemoveLight(pLight);
    }

    mNodes.erase(pNode);
}

Canavar::Engine::ScenePtr Canavar::Engine::NodeManager::GetScene(const QString& sceneName)
{
    ScenePtr pResult = nullptr;

    if (const auto it = mScenes.find(sceneName); it != mScenes.end())
    {
        pResult = it->second;
    }
    else
    {
        LOG_WARN("NodeManager::GetScene: Scene is not found: '{}'", sceneName.toStdString());
    }

    return pResult;
}

Canavar::Engine::ScenePtr Canavar::Engine::NodeManager::GetScene(ModelPtr pModel)
{
    return GetScene(pModel->GetModelName());
}

const std::map<QString, Canavar::Engine::ScenePtr>& Canavar::Engine::NodeManager::GetScenes() const
{
    return mScenes;
}

const std::set<Canavar::Engine::ModelPtr>& Canavar::Engine::NodeManager::GetModels() const
{
    return mModels;
}
