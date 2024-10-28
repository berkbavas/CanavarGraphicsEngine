#include "NodeManager.h"

#include "Canavar/Engine/Manager/LightManager.h"
#include "Canavar/Engine/Util/Logger.h"
#include "Canavar/Engine/Util/ModelImporter.h"

Canavar::Engine::NodeManager::NodeManager(QObject* parent) {}

void Canavar::Engine::NodeManager::Initialize()
{
    mLightManager = mManagerProvider->GetLightManager();

    mScenes = ModelImporter::Import("Resources/Models", { "*.obj", "*.blend", "*.fbx", "*.glb", "*.gltf" });

    mSky = std::make_shared<Sky>();
    mTerrain = std::make_shared<Terrain>();
    mHaze = std::make_shared<Haze>();

    AddNode(mSky);
    AddNode(mTerrain);
}

void Canavar::Engine::NodeManager::PostInitialize()
{
    LOG_DEBUG("NodeManager::PostInitialize: Initializing...");

    mSun = mLightManager->GetSun();

    mSky->Initialize();
    mTerrain->Initialize();

    for (const auto [name, pScene] : mScenes)
    {
        pScene->Initialize();
    }

    LOG_DEBUG("NodeManager::PostInitialize: Initialization is done.");
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

    pNode->SetNodeId(mCurrentNodeId++);
    mNodes.emplace(pNode);

    // > Add parent if not added yet
    if (const auto pParent = pNode->GetParent())
    {
        if (!mNodes.contains(pParent))
        {
            AddNode(pParent);
        }
    }

    // > Add children if not added yet
    const auto& children = pNode->GetChildren();

    for (const auto pChild : children)
    {
        if (!mNodes.contains(pChild))
        {
            AddNode(pChild);
        }
    }
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

    if (const auto pParent = pNode->GetParent())
    {
        pParent->RemoveChild(pNode);
    }
}

Canavar::Engine::ScenePtr Canavar::Engine::NodeManager::GetScene(const QString& sceneName) const
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

Canavar::Engine::ScenePtr Canavar::Engine::NodeManager::GetScene(ModelPtr pModel) const
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

const std::set<Canavar::Engine::NodePtr>& Canavar::Engine::NodeManager::GetNodes() const
{
    return mNodes;
}

Canavar::Engine::NodePtr Canavar::Engine::NodeManager::GetNodeById(uint32_t nodeId) const
{
    for (const auto& pNode : mNodes)
    {
        if (pNode->GetNodeId() == nodeId)
        {
            return pNode;
        }
    }

    return nullptr;
}

Canavar::Engine::MeshPtr Canavar::Engine::NodeManager::GetMeshById(ModelPtr pModel, uint32_t meshId) const
{
    if (const auto pScene = GetScene(pModel))
    {
        return pScene->GetMesh(meshId);
    }

    return nullptr;
}
