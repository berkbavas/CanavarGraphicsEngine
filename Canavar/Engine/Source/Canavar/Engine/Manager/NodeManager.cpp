#include "NodeManager.h"

#include "Canavar/Engine/Manager/LightManager.h"
#include "Canavar/Engine/Util/Logger.h"
#include "Canavar/Engine/Util/ModelImporter.h"

Canavar::Engine::NodeManager::NodeManager(QObject* parent)
    : Manager(parent)
{}

void Canavar::Engine::NodeManager::Initialize()
{
    mLightManager = mManagerProvider->GetLightManager();

    mScenes = ModelImporter::Import("Resources/Models", { "*.obj", "*.blend", "*.fbx", "*.glb", "*.gltf" });

    mSky = std::make_shared<Sky>();
    mTerrain = std::make_shared<Terrain>();
    mHaze = std::make_shared<Haze>();

    mSun = std::make_shared<DirectionalLight>();
    mSun->SetDirection(QVector3D(1, -1, 0).normalized());

    AddNode(mSky);
    AddNode(mTerrain);
    AddNode(mHaze);
    AddNode(mSun);
}

void Canavar::Engine::NodeManager::PostInitialize()
{
    LOG_DEBUG("NodeManager::PostInitialize: Initializing...");

    mSky->Initialize();
    mTerrain->Initialize();
    mHaze->Initialize();

    for (const auto& [name, pScene] : mScenes)
    {
        pScene->Initialize();
    }

    LOG_DEBUG("NodeManager::PostInitialize: Initialization is done.");
}

void Canavar::Engine::NodeManager::AddNode(NodePtr pNode)
{
    // TODO: Log

    pNode->SetNodeId(mCurrentNodeId++);
    mNodes.emplace(pNode);

    if (const auto pLight = std::dynamic_pointer_cast<Light>(pNode))
    {
        mLightManager->AddLight(pLight);
    }

    if (const auto pObject = std::dynamic_pointer_cast<Object>(pNode))
    {
        mObjects.emplace(pObject);

        // > Add parent if not added yet
        if (const auto pParent = pObject->GetParent())
        {
            if (mNodes.contains(pParent) == false)
            {
                AddNode(pParent);
            }
        }

        // > Add children if not added yet
        const auto& children = pObject->GetChildren();

        for (const auto& pChild : children)
        {
            if (mNodes.contains(pChild) == false)
            {
                AddNode(pChild);
            }
        }

        if (const auto pModel = std::dynamic_pointer_cast<Model>(pObject))
        {
            mModels.emplace(pModel);
        }
    }
}

void Canavar::Engine::NodeManager::RemoveNode(NodePtr pNode)
{
    // TODO: Log

    mNodes.erase(pNode);

    if (const auto pLight = std::dynamic_pointer_cast<Light>(pNode))
    {
        mLightManager->RemoveLight(pLight);
    }

    if (const auto pObject = std::dynamic_pointer_cast<Object>(pNode))
    {
        mObjects.erase(pObject);

        if (const auto pParent = pObject->GetParent())
        {
            pParent->RemoveChild(pObject);
        }

        if (const auto pModel = std::dynamic_pointer_cast<Model>(pObject))
        {
            mModels.erase(pModel);
        }
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

const std::set<Canavar::Engine::ObjectPtr>& Canavar::Engine::NodeManager::GetObjects() const
{
    return mObjects;
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
