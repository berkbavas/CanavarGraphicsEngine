#include "NodeManager.h"
#include "CameraManager.h"
#include "Config.h"
#include "DummyCamera.h"
#include "DummyNode.h"
#include "FirecrackerEffect.h"
#include "FreeCamera.h"
#include "Haze.h"
#include "Helper.h"
#include "LightManager.h"
#include "Model.h"
#include "ModelDataManager.h"
#include "NozzleEffect.h"
#include "PersecutorCamera.h"
#include "SelectableNodeRenderer.h"
#include "Sky.h"
#include "Sun.h"
#include "Terrain.h"
#include "LightningStrikeGenerator.h"
#include "LightningStrikeAttractor.h"
#include "LightningStrikeSpherical.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

Canavar::Engine::NodeManager::NodeManager()
    : Manager()
    , mNumberOfNodes(0)

{
    mTypeToName.insert(Node::NodeType::DummyCamera, "Dummy Camera");
    mTypeToName.insert(Node::NodeType::DummyNode, "Dummy Node");
    mTypeToName.insert(Node::NodeType::FreeCamera, "Free Camera");
    mTypeToName.insert(Node::NodeType::Model, "Model");
    mTypeToName.insert(Node::NodeType::PointLight, "Point Light");
    mTypeToName.insert(Node::NodeType::NozzleEffect, "Nozzle Effect");
    mTypeToName.insert(Node::NodeType::FirecrackerEffect, "Firecracker Effect");
    mTypeToName.insert(Node::NodeType::PersecutorCamera, "Persecutor Camera");
    mTypeToName.insert(Node::NodeType::LightningStrikeGenerator, "Lightning Strike Generator");
    mTypeToName.insert(Node::NodeType::LightningStrikeAttractor, "Lightning Strike Attractor");
    mTypeToName.insert(Node::NodeType::LightningStrikeSpherical, "Lightning Strike Spherical");
}

bool Canavar::Engine::NodeManager::Init()
{
    mCameraManager = CameraManager::Instance();
    mLightManager = LightManager::Instance();
    mModelDataManager = ModelDataManager::Instance();

    mNodes << Sun::Instance();
    Sun::Instance()->mID = mNumberOfNodes;
    mNumberOfNodes++;

    mNodes << Sky::Instance();
    Sky::Instance()->mID = mNumberOfNodes;
    mNumberOfNodes++;

    mNodes << Haze::Instance();
    Haze::Instance()->mID = mNumberOfNodes;
    mNumberOfNodes++;

    mNodes << Terrain::Instance();
    Terrain::Instance()->mID = mNumberOfNodes;
    mNumberOfNodes++;

    return true;
}

void Canavar::Engine::NodeManager::PostInit()
{
    auto doc = Helper::LoadJson(Config::Instance()->GetWorldFilePath());

    if (doc.isNull() || doc.isNull())
    {
        qCritical() << "World json at" << Config::Instance()->GetWorldFilePath() << "is not valid.";
        return;
    }

    auto array = doc["nodes"].toArray();

    QMap<QString, QString> childToParentMap;

    for (const auto& e : array)
    {
        auto object = e.toObject();

        if (!object["parent"].toString().isEmpty())
            childToParentMap.insert(object["uuid"].toString(), object["parent"].toString());

        Node::NodeType type = (Node::NodeType)object["type"].toInt();

        switch (type)
        {
        case Node::NodeType::DummyNode:
        case Node::NodeType::FreeCamera:
        case Node::NodeType::DummyCamera:
        case Node::NodeType::PointLight:
        case Node::NodeType::NozzleEffect:
        case Node::NodeType::FirecrackerEffect:
        case Node::NodeType::PersecutorCamera:
            CreateNode(type, object["name"].toString())->FromJson(object);
            break;

        case Node::NodeType::Model:
            CreateModel(object["model_name"].toString(), object["name"].toString())->FromJson(object);
            break;
        case Node::NodeType::Sun:
            Sun::Instance()->FromJson(object);
            break;
        case Node::NodeType::Sky:
            Sky::Instance()->FromJson(object);
            break;
        case Node::NodeType::Haze:
            Haze::Instance()->FromJson(object);
            break;
        case Node::NodeType::Terrain:
            Terrain::Instance()->FromJson(object);
            break;
        default:
            qWarning() << Q_FUNC_INFO << "Unknown node type: " << (int)type;
            break;
        }
    }

    // Set parents
    auto childUUIDs = childToParentMap.keys();
    for (const auto& childUUID : childUUIDs)
    {
        Node* node = GetNodeByUUID(childUUID);
        node->SetParent(GetNodeByUUID(childToParentMap.value(childUUID)));
    }
}

Canavar::Engine::Node* Canavar::Engine::NodeManager::CreateNode(Node::NodeType type, const QString& name)
{
    Node* node = nullptr;
    switch (type)
    {
    case Node::NodeType::DummyNode: {
        node = new DummyNode;
        break;
    }
    case Node::NodeType::Model: {
        qWarning() << "Use createModel() method instead!";
        break;
    }
    case Node::NodeType::FreeCamera: {
        node = new FreeCamera;
        mCameraManager->AddCamera(dynamic_cast<FreeCamera*>(node));
        break;
    }
    case Node::NodeType::DummyCamera: {
        node = new DummyCamera;
        mCameraManager->AddCamera(dynamic_cast<DummyCamera*>(node));
        break;
    }
    case Node::NodeType::PointLight: {
        node = new PointLight;
        mLightManager->AddLight(dynamic_cast<Light*>(node));
        break;
    }
    case Node::NodeType::NozzleEffect: {
        NozzleEffect* effect = new NozzleEffect;
        effect->Create();
        node = effect;
        break;
    }
    case Node::NodeType::FirecrackerEffect: {
        FirecrackerEffect* effect = new FirecrackerEffect;
        effect->Create();
        node = effect;
        break;
    }
    case Node::NodeType::PersecutorCamera: {
        node = new PersecutorCamera;
        mCameraManager->AddCamera(dynamic_cast<PersecutorCamera*>(node));
        break;
    }
    case Node::NodeType::LightningStrikeGenerator: {
        auto* lsg = new LightningStrikeGenerator;
        node = lsg;
        break;
    }
    case Node::NodeType::LightningStrikeAttractor: {
        auto* lsg = new LightningStrikeAttractor;
        node = lsg;
        break;
    }
    case Node::NodeType::LightningStrikeSpherical: {
        auto* lsg = new LightningStrikeSpherical;
        node = lsg;
        break;
    }
    default: {
        qWarning() << Q_FUNC_INFO << "Implement construction algorithm for this NodeType:" << (int)type;
        return nullptr;
    }
    }

    if (node)
    {
        AssignName(node, name);
        node->mID = mNumberOfNodes;
        mNodes << node;
        mNumberOfNodes++;

        emit NodeCreated(node);
    }

    return node;
}

Canavar::Engine::Model* Canavar::Engine::NodeManager::CreateModel(const QString& modelName, const QString& name)
{
    Model* model = new Model(modelName);

    AssignName(model, name);
    model->mID = mNumberOfNodes;
    mNodes << model;
    mNumberOfNodes++;

    emit NodeCreated(model);

    return model;
}

void Canavar::Engine::NodeManager::RemoveNode(Node* node)
{
    if (node == nullptr)
        return;

    switch (node->GetType())
    {
    case Node::NodeType::Model:
    case Node::NodeType::DummyNode:
    case Node::NodeType::NozzleEffect:
    case Node::NodeType::FirecrackerEffect: {
        // TODO: What will happen to node's children?
        if (node->mParent)
            node->mParent->RemoveChild(node);

        for (auto& child : node->GetChildren())
            child->SetParent(nullptr);

        mNodes.removeAll(node);
        node->deleteLater();
        break;
    }
    case Node::NodeType::PersecutorCamera:
    case Node::NodeType::DummyCamera:
    case Node::NodeType::FreeCamera: {
        if (node->mParent)
            node->mParent->RemoveChild(node);

        for (auto& child : node->GetChildren())
            child->SetParent(nullptr);

        mCameraManager->RemoveCamera(dynamic_cast<Camera*>(node));
        mNodes.removeAll(node);
        node->deleteLater();
        break;
    }
    case Node::NodeType::PointLight: {
        if (node->mParent)
            node->mParent->RemoveChild(node);

        for (auto& child : node->GetChildren())
            child->SetParent(nullptr);

        mLightManager->RemoveLight(dynamic_cast<Light*>(node));
        mNodes.removeAll(node);
        node->deleteLater();
        break;
    }
    default: {
        qWarning() << Q_FUNC_INFO << "Unknown Node. Implement deletion algorithm for this NodeType:" << (int)node->GetType();
        break;
    }
    }
}

Canavar::Engine::Node* Canavar::Engine::NodeManager::GetNodeByID(int ID)
{
    for (const auto& node : mNodes)
        if (node->GetID() == ID)
            return node;

    return nullptr;
}

Canavar::Engine::Node* Canavar::Engine::NodeManager::GetNodeByUUID(const QString& uuid)
{
    for (const auto& node : mNodes)
        if (node->GetUUID() == uuid)
            return node;

    return nullptr;
}

Canavar::Engine::Node* Canavar::Engine::NodeManager::GetNodeByName(const QString& name)
{
    for (const auto& node : mNodes)
        if (node->GetName() == name)
            return node;

    return nullptr;
}

Canavar::Engine::NodeManager* Canavar::Engine::NodeManager::Instance()
{
    static NodeManager instance;
    return &instance;
}

const QList<Canavar::Engine::Node*>& Canavar::Engine::NodeManager::GetNodes() const
{
    return mNodes;
}

void Canavar::Engine::NodeManager::ToJson(QJsonObject& object)
{
    QJsonArray array;

    for (auto& node : mNodes)
    {
        if (node->GetExcludeFromExport())
            continue;

        QJsonObject nodeJson;
        node->ToJson(nodeJson);
        array.append(nodeJson);
    }

    object.insert("nodes", array);
}

void Canavar::Engine::NodeManager::AssignName(Node* node, const QString& name)
{
    QString newName = name;

    if (newName.isEmpty())
    {
        newName = mTypeToName.value(node->mType);

        if (auto model = dynamic_cast<Model*>(node))
            newName += " " + model->GetModelName();

        node->SetName(newName);
    }
    else
        node->SetName(name);
}

Canavar::Engine::Node* Canavar::Engine::NodeManager::GetNodeByScreenPosition(int x, int y)
{
    auto info = SelectableNodeRenderer::Instance()->GetNodeInfoByScreenPosition(x, y);

    return GetNodeByID(info.nodeID);
}