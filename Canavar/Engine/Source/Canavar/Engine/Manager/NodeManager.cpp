#include "NodeManager.h"

#include "Canavar/Engine/Manager/CameraManager.h"
#include "Canavar/Engine/Manager/LightManager.h"
#include "Canavar/Engine/Util/Logger.h"
#include "Canavar/Engine/Util/ModelImporter.h"
#include "Canavar/Engine/Util/Util.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

Canavar::Engine::NodeManager::NodeManager(QObject* parent)
    : Manager(parent)
{}

void Canavar::Engine::NodeManager::Initialize()
{
    mLightManager = mManagerProvider->GetLightManager();

    mScenes = ModelImporter::Import(MODELS_FOLDER, { "*.obj", "*.blend", "*.fbx", "*.glb", "*.gltf" });

    mSky = std::make_shared<Sky>();
    mTerrain = std::make_shared<Terrain>();
    mHaze = std::make_shared<Haze>();
    mSun = std::make_shared<Sun>();

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

    mFreeCamera = mManagerProvider->GetCameraManager()->GetFreeCamera();

    for (const auto& [name, pScene] : mScenes)
    {
        pScene->Initialize();
    }

    LOG_DEBUG("NodeManager::PostInitialize: Initialization is done.");
}

void Canavar::Engine::NodeManager::AddNode(NodePtr pNode)
{
    if (pNode == nullptr)
    {
        LOG_FATAL("NodeManager::AddNode: pNode is nullptr. Returing...");
        return;
    }

    LOG_DEBUG("NodeManager::AddNode: > I will try to add this Node to my list. Node is '{}' at {}", pNode->GetNodeName().toStdString(), PRINT_ADDRESS(pNode.get()));

    if (mNodes.contains(pNode))
    {
        LOG_WARN("NodeManager::AddNode: This Node has been added already. Returning...");
        return;
    }

    pNode->SetNodeId(mCurrentNodeId++);
    mNodes.push_back(pNode);

    if (const auto pLight = std::dynamic_pointer_cast<Light>(pNode))
    {
        LOG_DEBUG("NodeManager::AddNode: This Node is Light. I am also adding it to LightManager's list.");
        mLightManager->AddLight(pLight);
    }

    if (const auto pObject = std::dynamic_pointer_cast<Object>(pNode))
    {
        LOG_DEBUG("NodeManager::AddNode: This Node is an Object. I am adding it to Object list as well.");

        mObjects.push_back(pObject);

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

        if (pObject->GetChildren().size() > 0)
        {
            LOG_DEBUG("NodeManager::AddNode: Let's see if this Object has any children which are not added to my list yet.");
        }

        for (auto it = pObject->GetChildren().begin(); it != pObject->GetChildren().end(); ++it)
        {
            const auto pChild = *it;

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
            mModels.push_back(pModel);
        }
    }

    LOG_DEBUG("NodeManager::AddNode: < I have added this Node: '{}' at {}", pNode->GetNodeName().toStdString(), PRINT_ADDRESS(pNode.get()));
}

void Canavar::Engine::NodeManager::RemoveNode(NodePtr pNode)
{
    LOG_DEBUG("NodeManager::RemoveNode: > I will remove this Node: '{}' at {}", pNode->GetNodeName().toStdString(), PRINT_ADDRESS(pNode.get()));

    int count = mNodes.removeAll(pNode);

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

        mObjects.removeAll(pObject);

        auto children = pObject->GetChildren();

        for (auto it = children.begin(); it != children.end(); ++it)
        {
            if (const auto pChild = *it)
            {
                LOG_DEBUG("NodeManager::RemoveNode: Removing this Object's child: '{}' at {}. This is a recursive call.", pChild->GetNodeName().toStdString(), PRINT_ADDRESS(pChild.get()));
                RemoveNode(pChild);
            }
        }

        if (const auto pParent = pObject->GetParent())
        {
            LOG_DEBUG("NodeManager::RemoveNode: Object has a parent. Removing this Object its parent's child list.");
            pParent->RemoveChild(pObject);
        }

        if (const auto pModel = std::dynamic_pointer_cast<Model>(pObject))
        {
            LOG_DEBUG("NodeManager::RemoveNode: Object is a Model. Removing it from Model list as well.");
            mModels.removeAll(pModel);
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

    return pResult;
}

Canavar::Engine::ScenePtr Canavar::Engine::NodeManager::GetScene(ModelPtr pModel) const
{
    return GetScene(pModel->GetSceneName());
}

const std::map<QString, Canavar::Engine::ScenePtr>& Canavar::Engine::NodeManager::GetScenes() const
{
    return mScenes;
}

const QList<Canavar::Engine::ModelPtr>& Canavar::Engine::NodeManager::GetModels() const
{
    return mModels;
}

const QList<Canavar::Engine::NodePtr>& Canavar::Engine::NodeManager::GetNodes() const
{
    return mNodes;
}

const QList<Canavar::Engine::ObjectPtr>& Canavar::Engine::NodeManager::GetObjects() const
{
    return mObjects;
}

Canavar::Engine::MeshPtr Canavar::Engine::NodeManager::GetMeshById(ModelPtr pModel, uint32_t meshId) const
{
    if (const auto pScene = GetScene(pModel))
    {
        return pScene->GetMesh(meshId);
    }

    return nullptr;
}

void Canavar::Engine::NodeManager::ExportNodes(const QString& path)
{
    QJsonDocument document;
    QJsonObject root;
    QJsonArray array;

    for (const auto& pNode : mNodes)
    {
        QJsonObject object;
        pNode->ToJson(object);
        array.append(object);
    }

    root.insert("nodes", array);
    document.setObject(root);

    const auto content = document.toJson(QJsonDocument::Indented);

    if (Util::WriteTextToFile(path, content))
    {
        LOG_INFO("NodeManager::ExportNodes: Nodes are successfully exported.");
    }
    else
    {
        LOG_FATAL("NodeManager::ExportNodes: Could not export nodes.");
    }
}

void Canavar::Engine::NodeManager::ImportNodes(const QString& path)
{
    QJsonDocument document = Util::ReadJson(path);
    QJsonObject root = document.object();
    QJsonArray array = root["nodes"].toArray();

    std::map<QString, NodePtr> nodes;
    std::map<QString, QJsonObject> objects;

    for (const auto elememt : array)
    {
        QJsonObject object = elememt.toObject();
        QString nodeType = object["node_type"].toString();
        QString uuid = object["uuid"].toString();

        LOG_DEBUG("NodeManager::ImportNodes: uuid: {}, node_type: {}", uuid.toStdString(), nodeType.toStdString());

        if (nodeType.isNull() || nodeType.isEmpty())
        {
            continue;
        }

        else if (nodeType == Model::NODE_TYPE)
        {
            QString sceneName = object["scene_name"].toString();

            if (sceneName.isNull() == false && sceneName.isEmpty() == false)
            {
                ModelPtr pModel = std::make_shared<Model>(sceneName);
                nodes.insert(std::pair(uuid, pModel));
                objects.insert(std::pair(uuid, object));
            }
        }
        else
        {
            const auto factory = Object::OBJECT_FACTORIES[nodeType];

            if (factory != nullptr)
            {
                NodePtr pNode = factory();

                if (pNode)
                {
                    nodes.insert(std::pair(uuid, pNode));
                    objects.insert(std::pair(uuid, object));
                }
            }
        }
    }

    for (const auto element : array)
    {
        QJsonObject object = element.toObject();
        QString nodeType = object["node_type"].toString();
        QString uuid = object["uuid"].toString();

        if (nodeType == QString(Haze::NODE_TYPE))
        {
            mHaze->FromJson(object, nodes);
        }
        else if (nodeType == Sky::NODE_TYPE)
        {
            mSky->FromJson(object, nodes);
        }
        else if (nodeType == Terrain::NODE_TYPE)
        {
            mTerrain->FromJson(object, nodes);
        }
        else if (nodeType == Sun::NODE_TYPE)
        {
            mSun->FromJson(object, nodes);
        }
        else if (const auto pNode = nodes[uuid])
        {
            pNode->FromJson(object, nodes);
        }
    }

    for (const auto& [uuid, node] : nodes)
    {
        AddNode(node);
    }

    LOG_DEBUG("NodeManager::ImportNodes: {} node(s) have been imported.", nodes.size());
}

void Canavar::Engine::NodeManager::RemoveAllNodes()
{
    // Clear all except Haze, Terrain, Sky, Sun
    mNodes.clear();
    mObjects.clear();
    mModels.clear();

    AddNode(mSky);
    AddNode(mTerrain);
    AddNode(mHaze);
    AddNode(mSun);
}
