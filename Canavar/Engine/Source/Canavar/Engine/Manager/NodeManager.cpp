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
    mSun->SetNodeName("Sun");
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
    LOG_DEBUG("NodeManager::AddNode: > I will try to add this Node to my list. Node is '{}' at {}", pNode->GetNodeName().toStdString(), PRINT_ADDRESS(pNode.get()));

    if (mNodes.contains(pNode))
    {
        LOG_WARN("NodeManager::AddNode: This Node has been added already. Returning...");
        return;
    }

    pNode->SetNodeId(mCurrentNodeId++);
    mNodes.emplace(pNode);

    if (const auto pLight = std::dynamic_pointer_cast<Light>(pNode))
    {
        LOG_DEBUG("NodeManager::AddNode: This Node is Light. I am also adding it to LightManager's list.");
        mLightManager->AddLight(pLight);
    }

    if (const auto pObject = std::dynamic_pointer_cast<Object>(pNode))
    {
        LOG_DEBUG("NodeManager::AddNode: This Node is an Object. I am adding it to Object list as well.");

        mObjects.emplace(pObject);

        if (const auto pParent = pObject->GetParent())
        {
            LOG_DEBUG("NodeManager::AddNode: This Object has a parent. Let's see if its parent added to my list. Parent is '{}' at {}", pParent->GetNodeName().toStdString(), PRINT_ADDRESS(pParent.get()));

            if (mNodes.contains(pParent) == false)
            {
                LOG_DEBUG("NodeManager::AddNode: Object's parent is not on my list. I am calling AddNode on this parent. This is a recursive call.");
                AddNode(pParent);
            }
            else
            {
                LOG_DEBUG("NodeManager::AddNode: Parent is on my list.");
            }
        }

        const auto& children = pObject->GetChildren();

        if (children.size() > 0)
        {
            LOG_DEBUG("NodeManager::AddNode: Let's see if this Object has any children which are not added to my list yet.");
        }

        for (const auto& pChild : children)
        {
            if (mNodes.contains(pChild) == false)
            {
                LOG_DEBUG("NodeManager::AddNode: Object has a child which is not added to my list. Child is '{}' at {}", pChild->GetNodeName().toStdString(), PRINT_ADDRESS(pChild.get()));
                LOG_DEBUG("NodeManager::AddNode: I am calling AddNode on this child. This is a recursive call.");
                AddNode(pChild);
            }
            else
            {
                LOG_DEBUG("NodeManager::AddNode: This child is on my list: '{}' at {}", pChild->GetNodeName().toStdString(), PRINT_ADDRESS(pChild.get()));
            }
        }

        if (const auto pModel = std::dynamic_pointer_cast<Model>(pObject))
        {
            LOG_DEBUG("NodeManager::AddNode: This Object is a Model. I am adding it to Model list as well");
            mModels.emplace(pModel);
        }
    }

    LOG_DEBUG("NodeManager::AddNode: < I have added this Node: '{}' at {}", pNode->GetNodeName().toStdString(), PRINT_ADDRESS(pNode.get()));
}

void Canavar::Engine::NodeManager::RemoveNode(NodePtr pNode)
{
    LOG_DEBUG("NodeManager::RemoveNode: > I will remove this Node: '{}' at {}", pNode->GetNodeName().toStdString(), PRINT_ADDRESS(pNode.get()));

    int count = mNodes.erase(pNode);

    if (count == 0)
    {
        LOG_WARN("NodeManager::RemoveNode: Node was not in my list at all. WTF?");
    }

    if (const auto pLight = std::dynamic_pointer_cast<Light>(pNode))
    {
        LOG_DEBUG("NodeManager::RemoveNode: This Node is a Light, so I will remove it from LightManager's list as well.");

        mLightManager->RemoveLight(pLight);
    }

    if (const auto pObject = std::dynamic_pointer_cast<Object>(pNode))
    {
        LOG_DEBUG("NodeManager::RemoveNode: This node is an Object. I am removing it from Object list.");

        mObjects.erase(pObject);

        const auto& children = pObject->GetChildren();

        for (const auto& pChild : children)
        {
            LOG_DEBUG("NodeManager::RemoveNode: Removing this Object's child: '{}' at {}. This is a recursive call.", pChild->GetNodeName().toStdString(), PRINT_ADDRESS(pChild.get()));
            RemoveNode(pChild);
        }

        if (const auto pParent = pObject->GetParent())
        {
            LOG_DEBUG("NodeManager::RemoveNode: Object has a parent. Removing this Object its parent's child list.");
            pParent->RemoveChild(pObject);
        }

        if (const auto pModel = std::dynamic_pointer_cast<Model>(pObject))
        {
            LOG_DEBUG("NodeManager::RemoveNode: Object is a Model. Removing it from Model list as well.");
            mModels.erase(pModel);
        }
    }

    LOG_DEBUG("NodeManager::RemoveNode: < Node is removed.");
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
        //LOG_WARN("NodeManager::GetScene: Scene is not found: '{}'", sceneName.toStdString());
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
