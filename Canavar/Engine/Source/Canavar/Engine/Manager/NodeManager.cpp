#include "NodeManager.h"

#include "Canavar/Engine/Camera/PerspectiveCamera.h"
#include "Canavar/Engine/GlobalNode/Haze/Haze.h"
#include "Canavar/Engine/GlobalNode/Sky/Sky.h"
#include "Canavar/Engine/GlobalNode/Terrain/Terrain.h"
#include "Canavar/Engine/Manager/Renderer.h"
#include "Canavar/Engine/Util/Logger.h"
#include "Canavar/Engine/Util/Util.h"

#include <vector>

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QMap>
#include <QUuid>

Canavar::Engine::NodeManager::NodeManager(Renderer *pRenderer)
    : mRenderer(pRenderer)
{}

void Canavar::Engine::NodeManager::Initialize()
{
    mLightManager = mRenderer->GetLightManager();
}

void Canavar::Engine::NodeManager::RemoveNode(Node *pNode)
{
    // ── Clean up hierarchy links ──────────────────────────────────────────────
    // Detach from parent so the parent's child list has no dangling pointer.
    if (pNode->GetParent())
    {
        pNode->SetParent(nullptr);
    }

    // Orphan direct children so they become root nodes (prevents dangling parent ptrs).
    // Copy the list first — SetParent modifies pNode->mChildren during iteration.
    const std::vector<Node *> ChildrenCopy = pNode->GetChildren();
    for (Node *pChild : ChildrenCopy)
    {
        pChild->SetParent(nullptr);
    }

    // ── Remove from manager lists ─────────────────────────────────────────────
    mObjects.remove(dynamic_cast<Object *>(pNode));
    mPointLights.remove(dynamic_cast<PointLight *>(pNode));
    mDirectionalLights.remove(dynamic_cast<DirectionalLight *>(pNode));
    mLightManager->RemoveLight(dynamic_cast<Light *>(pNode));
    mCameras.remove(dynamic_cast<Camera *>(pNode));
    mLights.remove(dynamic_cast<Light *>(pNode));
    mTexturedModels.remove(dynamic_cast<TexturedModel *>(pNode));
    mPrimitiveModels.remove(dynamic_cast<PrimitiveModel *>(pNode));
    mGlobalNodes.remove(dynamic_cast<GlobalNode *>(pNode));
    mDummyObjects.remove(dynamic_cast<DummyObject *>(pNode));

    // Remove the node from the main list of nodes. This will also delete the node since we are using unique_ptr.
    mNodes.remove_if([pNode](const NodePtr &ptr) { return ptr.get() == pNode; });
}

void Canavar::Engine::NodeManager::AddCamera(Camera *pCamera)
{
    mCameras.push_back(pCamera);
    mObjects.push_back(pCamera);
}

void Canavar::Engine::NodeManager::AddLight(Light *pLight)
{
    mLights.push_back(pLight);
    mObjects.push_back(pLight);

    // Add the light to the LightManager for uniform management.
    mLightManager->AddLight(pLight);
}

void Canavar::Engine::NodeManager::AddTexturedModel(TexturedModel *pTexturedModel)
{
    mTexturedModels.push_back(pTexturedModel);
    mObjects.push_back(pTexturedModel);
}

void Canavar::Engine::NodeManager::AddPrimitiveModel(PrimitiveModel *pPrimitiveModel)
{
    mPrimitiveModels.push_back(pPrimitiveModel);
    mObjects.push_back(pPrimitiveModel);
}

void Canavar::Engine::NodeManager::AddGlobalNode(GlobalNode *pGlobalNode)
{
    mGlobalNodes.push_back(pGlobalNode);
}

void Canavar::Engine::NodeManager::AddDummyObject(DummyObject *pDummyObject)
{
    mDummyObjects.push_back(pDummyObject);
    mObjects.push_back(pDummyObject);
}

const std::list<Canavar::Engine::NodePtr> &Canavar::Engine::NodeManager::GetNodes() const
{
    return mNodes;
}

const std::list<Canavar::Engine::Object *> &Canavar::Engine::NodeManager::GetObjects() const
{
    return mObjects;
}

const std::list<Canavar::Engine::PointLight *> &Canavar::Engine::NodeManager::GetPointLights() const
{
    return mPointLights;
}

const std::list<Canavar::Engine::DirectionalLight *> &Canavar::Engine::NodeManager::GetDirectionalLights() const
{
    return mDirectionalLights;
}

const std::list<Canavar::Engine::Camera *> &Canavar::Engine::NodeManager::GetCameras() const
{
    return mCameras;
}
const std::list<Canavar::Engine::Light *> &Canavar::Engine::NodeManager::GetLights() const
{
    return mLights;
}

const std::list<Canavar::Engine::TexturedModel *> &Canavar::Engine::NodeManager::GetTexturedModels() const
{
    return mTexturedModels;
}

const std::list<Canavar::Engine::PrimitiveModel *> &Canavar::Engine::NodeManager::GetPrimitiveModels() const
{
    return mPrimitiveModels;
}

const std::list<Canavar::Engine::GlobalNode *> &Canavar::Engine::NodeManager::GetGlobalNodes() const
{
    return mGlobalNodes;
}

const std::list<Canavar::Engine::DummyObject *> &Canavar::Engine::NodeManager::GetDummyObjects() const
{
    return mDummyObjects;
}

// ─────────────────────────────────────────────────────────────────────────────
// ImportNodes — static JSON helpers
// ─────────────────────────────────────────────────────────────────────────────

static QVector3D SReadVec3(const QJsonObject &O)
{
    return { (float) O["x"].toDouble(), (float) O["y"].toDouble(), (float) O["z"].toDouble() };
}

static QVector3D SReadColor(const QJsonObject &O)
{
    return { (float) O["r"].toDouble(), (float) O["g"].toDouble(), (float) O["b"].toDouble() };
}

static QQuaternion SReadQuat(const QJsonObject &O)
{
    return QQuaternion((float) O["w"].toDouble(), (float) O["x"].toDouble(), (float) O["y"].toDouble(), (float) O["z"].toDouble());
}

static void SApplyObjectProps(Canavar::Engine::Object *pObj, const QJsonObject &O)
{
    if (O.contains("position"))
        pObj->SetPosition(SReadVec3(O["position"].toObject()));
    if (O.contains("rotation"))
        pObj->SetRotation(SReadQuat(O["rotation"].toObject()));
    if (O.contains("scale"))
        pObj->SetScale(SReadVec3(O["scale"].toObject()));
    if (O.contains("visible"))
        pObj->SetVisible(O["visible"].toBool(true));
}

static void SApplyCameraProps(Canavar::Engine::PerspectiveCamera *pCam, const QJsonObject &O)
{
    if (O.contains("z_near"))
        pCam->SetZNear((float) O["z_near"].toDouble());
    if (O.contains("z_far"))
        pCam->SetZFar((float) O["z_far"].toDouble());
    if (O.contains("vertical_fov"))
        pCam->SetVerticalFov((float) O["vertical_fov"].toDouble());
}

bool Canavar::Engine::NodeManager::ImportNodes(const std::string &FilePath)
{
    const QJsonDocument Doc = Util::ReadJson(QString::fromStdString(FilePath));

    if (Doc.isNull() || !Doc.isObject())
    {
        LOG_WARN("NodeManager::ImportNodes: Failed to read or parse JSON file: {}", FilePath);
        return false;
    }

    const QJsonArray NodesArray = Doc.object()["nodes"].toArray();

    // ── First pass: create / find nodes ─────────────────────────────────────

    QMap<QString, Node *> UuidToNode;

    struct PendingParent
    {
        Node *pNode;
        QString ParentUuid;
    };
    QVector<PendingParent> PendingParents;

    for (const QJsonValue &Val : NodesArray)
    {
        if (!Val.isObject())
            continue;

        const QJsonObject NodeObj = Val.toObject();
        const QString TypeName = NodeObj["node_type_name"].toString();
        const QString NodeName = NodeObj["node_name"].toString();
        const QString UuidStr = NodeObj["uuid"].toString();

        Node *pNode = nullptr;

        if (TypeName == "Sky")
        {
            pNode = FindNodeByType<Sky>();
            if (auto *pSky = static_cast<Sky *>(pNode))
            {
                if (NodeObj.contains("enabled"))
                    pSky->SetEnabled(NodeObj["enabled"].toBool());
                if (NodeObj.contains("sun_intensity"))
                    pSky->SetSunIntensity((float) NodeObj["sun_intensity"].toDouble());
                if (NodeObj.contains("planet_radius"))
                    pSky->SetPlanetRadius((float) NodeObj["planet_radius"].toDouble());
                if (NodeObj.contains("atmosphere_radius"))
                    pSky->SetAtmosphereRadius((float) NodeObj["atmosphere_radius"].toDouble());
                if (NodeObj.contains("beta_rayleigh"))
                    pSky->SetBetaRayleigh(SReadVec3(NodeObj["beta_rayleigh"].toObject()));
                if (NodeObj.contains("beta_mie"))
                    pSky->SetBetaMie((float) NodeObj["beta_mie"].toDouble());
                if (NodeObj.contains("scale_height_r"))
                    pSky->SetScaleHeightR((float) NodeObj["scale_height_r"].toDouble());
                if (NodeObj.contains("scale_height_m"))
                    pSky->SetScaleHeightM((float) NodeObj["scale_height_m"].toDouble());
                if (NodeObj.contains("mie_g"))
                    pSky->SetMieG((float) NodeObj["mie_g"].toDouble());
                if (NodeObj.contains("horizon_offset"))
                    pSky->SetHorizonOffset((float) NodeObj["horizon_offset"].toDouble());
            }
        }
        else if (TypeName == "Haze")
        {
            Haze *pHaze = FindNodeByType<Haze>();
            if (pHaze)
            {
                if (NodeObj.contains("color"))
                    pHaze->SetColor(SReadColor(NodeObj["color"].toObject()));
                if (NodeObj.contains("density"))
                    pHaze->SetDensity((float) NodeObj["density"].toDouble());
                if (NodeObj.contains("gradient"))
                    pHaze->SetGradient((float) NodeObj["gradient"].toDouble());
                if (NodeObj.contains("enabled"))
                    pHaze->SetEnabled(NodeObj["enabled"].toBool());
                pNode = pHaze;
            }
        }
        else if (TypeName == "Sun")
        {
            // The primary directional light (Sun) is owned by the Renderer.
            DirectionalLight *pSun = mRenderer->GetSun();
            if (pSun)
            {
                if (NodeObj.contains("color"))
                    pSun->SetColor(SReadColor(NodeObj["color"].toObject()));
                if (NodeObj.contains("ambient"))
                    pSun->SetAmbient((float) NodeObj["ambient"].toDouble());
                if (NodeObj.contains("diffuse"))
                    pSun->SetDiffuse((float) NodeObj["diffuse"].toDouble());
                if (NodeObj.contains("specular"))
                    pSun->SetSpecular((float) NodeObj["specular"].toDouble());
                if (NodeObj.contains("radience"))
                    pSun->SetRadiance((float) NodeObj["radience"].toDouble()); // JSON key is misspelled
                if (NodeObj.contains("radiance"))
                    pSun->SetRadiance((float) NodeObj["radiance"].toDouble());
                if (NodeObj.contains("direction"))
                    pSun->SetDirection(SReadVec3(NodeObj["direction"].toObject()));
                if (NodeObj.contains("enabled"))
                    pSun->SetEnabled(NodeObj["enabled"].toBool());
                SApplyObjectProps(pSun, NodeObj);
                pNode = pSun;
            }
        }
        else if (TypeName == "Terrain")
        {
            Terrain *pTerrain = FindNodeByType<Terrain>();
            if (pTerrain)
            {
                if (NodeObj.contains("octaves"))
                    pTerrain->SetOctaves(NodeObj["octaves"].toInt());
                if (NodeObj.contains("amplitude"))
                    pTerrain->SetAmplitude((float) NodeObj["amplitude"].toDouble());
                if (NodeObj.contains("frequency"))
                    pTerrain->SetFrequency((float) NodeObj["frequency"].toDouble());
                if (NodeObj.contains("persistence"))
                    pTerrain->SetPersistence((float) NodeObj["persistence"].toDouble());
                if (NodeObj.contains("lacunarity"))
                    pTerrain->SetLacunarity((float) NodeObj["lacunarity"].toDouble());
                if (NodeObj.contains("tessellation_multiplier"))
                    pTerrain->SetTessellationMultiplier((float) NodeObj["tessellation_multiplier"].toDouble());
                if (NodeObj.contains("ambient"))
                    pTerrain->SetAmbient((float) NodeObj["ambient"].toDouble());
                if (NodeObj.contains("diffuse"))
                    pTerrain->SetDiffuse((float) NodeObj["diffuse"].toDouble());
                if (NodeObj.contains("specular"))
                    pTerrain->SetSpecular((float) NodeObj["specular"].toDouble());
                if (NodeObj.contains("shininess"))
                    pTerrain->SetShininess((float) NodeObj["shininess"].toDouble());
                if (NodeObj.contains("enabled"))
                    pTerrain->SetEnabled(NodeObj["enabled"].toBool());
                pNode = pTerrain;
            }
        }
        else if (TypeName == "FreeCamera")
        {
            FreeCamera *pCam = CreateNode<FreeCamera>();
            SApplyObjectProps(pCam, NodeObj);
            SApplyCameraProps(pCam, NodeObj);
            pNode = pCam;
        }
        else if (TypeName == "DummyCamera")
        {
            DummyCamera *pCam = CreateNode<DummyCamera>();
            SApplyObjectProps(pCam, NodeObj);
            SApplyCameraProps(pCam, NodeObj);
            pNode = pCam;
        }
        else if (TypeName == "PersecutorCamera")
        {
            PersecutorCamera *pCam = CreateNode<PersecutorCamera>();
            SApplyObjectProps(pCam, NodeObj);
            SApplyCameraProps(pCam, NodeObj);
            if (NodeObj.contains("distance"))
                pCam->SetDistance((float) NodeObj["distance"].toDouble());
            if (NodeObj.contains("yaw"))
                pCam->SetYaw((float) NodeObj["yaw"].toDouble());
            if (NodeObj.contains("pitch"))
                pCam->SetPitch((float) NodeObj["pitch"].toDouble());
            if (NodeObj.contains("angular_speed"))
                pCam->SetAngularSpeed((float) NodeObj["angular_speed"].toDouble());
            if (NodeObj.contains("linear_speed"))
                pCam->SetLinearSpeed((float) NodeObj["linear_speed"].toDouble());
            pNode = pCam;
        }
        else if (TypeName == "DummyObject")
        {
            DummyObject *pObj = CreateNode<DummyObject>();
            SApplyObjectProps(pObj, NodeObj);
            pNode = pObj;
        }
        else if (TypeName == "TexturedModel")
        {
            const QString SceneName = NodeObj["scene_name"].toString();
            if (SceneName.isEmpty())
            {
                LOG_WARN("NodeManager::ImportNodes: 'Model' node '{}' has no scene_name, skipping.", NodeName.toStdString());
                continue;
            }
            TexturedModel *pModel = CreateNode<TexturedModel>(SceneName);
            SApplyObjectProps(pModel, NodeObj);
            if (NodeObj.contains("color"))
                pModel->SetColor(SReadColor(NodeObj["color"].toObject()));
            // Handle both old (use_model_color) and new (use_color) field names
            if (NodeObj.contains("use_color"))
                pModel->SetUseColor(NodeObj["use_color"].toBool());
            if (NodeObj.contains("use_model_color"))
                pModel->SetUseColor(NodeObj["use_model_color"].toBool());
            pNode = pModel;
        }
        else if (TypeName == "DirectionalLight")
        {
            DirectionalLight *pLight = CreateNode<DirectionalLight>();
            SApplyObjectProps(pLight, NodeObj);
            if (NodeObj.contains("color"))
                pLight->SetColor(SReadColor(NodeObj["color"].toObject()));
            if (NodeObj.contains("ambient"))
                pLight->SetAmbient((float) NodeObj["ambient"].toDouble());
            if (NodeObj.contains("diffuse"))
                pLight->SetDiffuse((float) NodeObj["diffuse"].toDouble());
            if (NodeObj.contains("specular"))
                pLight->SetSpecular((float) NodeObj["specular"].toDouble());
            if (NodeObj.contains("radiance"))
                pLight->SetRadiance((float) NodeObj["radiance"].toDouble());
            if (NodeObj.contains("direction"))
                pLight->SetDirection(SReadVec3(NodeObj["direction"].toObject()));
            if (NodeObj.contains("enabled"))
                pLight->SetEnabled(NodeObj["enabled"].toBool());
            pNode = pLight;
        }
        else if (TypeName == "PointLight")
        {
            PointLight *pLight = CreateNode<PointLight>();
            SApplyObjectProps(pLight, NodeObj);
            if (NodeObj.contains("color"))
                pLight->SetColor(SReadColor(NodeObj["color"].toObject()));
            if (NodeObj.contains("ambient"))
                pLight->SetAmbient((float) NodeObj["ambient"].toDouble());
            if (NodeObj.contains("diffuse"))
                pLight->SetDiffuse((float) NodeObj["diffuse"].toDouble());
            if (NodeObj.contains("specular"))
                pLight->SetSpecular((float) NodeObj["specular"].toDouble());
            if (NodeObj.contains("constant"))
                pLight->SetConstant((float) NodeObj["constant"].toDouble());
            if (NodeObj.contains("linear"))
                pLight->SetLinear((float) NodeObj["linear"].toDouble());
            if (NodeObj.contains("quadratic"))
                pLight->SetQuadratic((float) NodeObj["quadratic"].toDouble());
            if (NodeObj.contains("enabled"))
                pLight->SetEnabled(NodeObj["enabled"].toBool());
            pNode = pLight;
        }
        else
        {
            LOG_WARN("NodeManager::ImportNodes: Unsupported node type '{}', skipping.", TypeName.toStdString());
            continue;
        }

        if (!pNode)
        {
            LOG_WARN("NodeManager::ImportNodes: Could not find or create node of type '{}', skipping.", TypeName.toStdString());
            continue;
        }

        if (!NodeName.isEmpty())
            pNode->SetNodeName(NodeName.toStdString());

        if (!UuidStr.isEmpty())
            UuidToNode[UuidStr] = pNode;

        if (NodeObj.contains("parent_uuid"))
            PendingParents.push_back({ pNode, NodeObj["parent_uuid"].toString() });
    }

    // ── Second pass: wire up parent-child relationships ──────────────────────

    for (const auto &PP : PendingParents)
    {
        auto It = UuidToNode.find(PP.ParentUuid);
        if (It != UuidToNode.end())
        {
            PP.pNode->SetParent(It.value());
        }
        else
        {
            LOG_WARN("NodeManager::ImportNodes: Parent UUID '{}' not found for node '{}'.", PP.ParentUuid.toStdString(), PP.pNode->GetNodeName());
        }
    }

    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
// ExportNodes — static JSON helpers
// ─────────────────────────────────────────────────────────────────────────────

static QJsonObject SWriteVec3(const QVector3D &V)
{
    return { { "x", (double) V.x() }, { "y", (double) V.y() }, { "z", (double) V.z() } };
}

static QJsonObject SWriteColor(const QVector3D &C)
{
    return { { "r", (double) C.x() }, { "g", (double) C.y() }, { "b", (double) C.z() } };
}

static QJsonObject SWriteQuat(const QQuaternion &Q)
{
    return { { "w", (double) Q.scalar() }, { "x", (double) Q.x() }, { "y", (double) Q.y() }, { "z", (double) Q.z() } };
}

static void SWriteObjectProps(QJsonObject &Obj, const Canavar::Engine::Object *pObj)
{
    Obj["position"] = SWriteVec3(pObj->GetPosition());
    Obj["rotation"] = SWriteQuat(pObj->GetRotation());
    Obj["scale"] = SWriteVec3(pObj->GetScale());
    Obj["visible"] = pObj->GetVisible();
}

static void SWritePerspCameraProps(QJsonObject &Obj, const Canavar::Engine::PerspectiveCamera *pCam)
{
    Obj["z_near"] = (double) pCam->GetZNear();
    Obj["z_far"] = (double) pCam->GetZFar();
    Obj["vertical_fov"] = (double) pCam->GetVerticalFov();
}

// ─────────────────────────────────────────────────────────────────────────────
// ExportNodes
// ─────────────────────────────────────────────────────────────────────────────

bool Canavar::Engine::NodeManager::ExportNodes(const std::string &FilePath)
{
    // Build a UUID string for every managed node so parent_uuid references are valid.
    // Nodes always have a UUID after the CreateNode change; the fallback handles any
    // legacy node that bypassed CreateNode.
    QMap<const Node *, QString> UuidMap;
    for (const auto &pNodePtr : mNodes)
    {
        const Node *p = pNodePtr.get();
        UuidMap[p] = p->GetUuid().isNull() ? QUuid::createUuid().toString(QUuid::WithoutBraces) : p->GetUuid().toString(QUuid::WithoutBraces);
    }

    QJsonArray NodesArray;

    for (const auto &pNodePtr : mNodes)
    {
        Node *pNode = pNodePtr.get();
        QJsonObject NodeObj;

        // ── Determine node type and write type-specific fields ─────────────────

        if (auto *pSky = dynamic_cast<Sky *>(pNode))
        {
            NodeObj["node_type_name"] = "Sky";
            NodeObj["enabled"] = pSky->GetEnabled();
            NodeObj["sun_intensity"] = (double) pSky->GetSunIntensity();
            NodeObj["planet_radius"] = (double) pSky->GetPlanetRadius();
            NodeObj["atmosphere_radius"] = (double) pSky->GetAtmosphereRadius();
            NodeObj["beta_rayleigh"] = SWriteVec3(pSky->GetBetaRayleigh());
            NodeObj["beta_mie"] = (double) pSky->GetBetaMie();
            NodeObj["scale_height_r"] = (double) pSky->GetScaleHeightR();
            NodeObj["scale_height_m"] = (double) pSky->GetScaleHeightM();
            NodeObj["mie_g"] = (double) pSky->GetMieG();
            NodeObj["horizon_offset"] = (double) pSky->GetHorizonOffset();
        }
        else if (auto *pHaze = dynamic_cast<Haze *>(pNode))
        {
            NodeObj["node_type_name"] = "Haze";
            NodeObj["color"] = SWriteColor(pHaze->GetColor());
            NodeObj["density"] = (double) pHaze->GetDensity();
            NodeObj["gradient"] = (double) pHaze->GetGradient();
            NodeObj["enabled"] = pHaze->GetEnabled();
        }
        else if (auto *pTerrain = dynamic_cast<Terrain *>(pNode))
        {
            NodeObj["node_type_name"] = "Terrain";
            NodeObj["octaves"] = pTerrain->GetOctaves();
            NodeObj["amplitude"] = (double) pTerrain->GetAmplitude();
            NodeObj["frequency"] = (double) pTerrain->GetFrequency();
            NodeObj["persistence"] = (double) pTerrain->GetPersistence();
            NodeObj["lacunarity"] = (double) pTerrain->GetLacunarity();
            NodeObj["tessellation_multiplier"] = (double) pTerrain->GetTessellationMultiplier();
            NodeObj["ambient"] = (double) pTerrain->GetAmbient();
            NodeObj["diffuse"] = (double) pTerrain->GetDiffuse();
            NodeObj["specular"] = (double) pTerrain->GetSpecular();
            NodeObj["shininess"] = (double) pTerrain->GetShininess();
            NodeObj["enabled"] = pTerrain->GetEnabled();
        }
        else if (auto *pDirLight = dynamic_cast<DirectionalLight *>(pNode))
        {
            // Distinguish the renderer's Sun from other directional lights.
            const bool IsSun = (pDirLight == mRenderer->GetSun());
            NodeObj["node_type_name"] = IsSun ? "Sun" : "DirectionalLight";
            NodeObj["color"] = SWriteColor(pDirLight->GetColor());
            NodeObj["ambient"] = (double) pDirLight->GetAmbient();
            NodeObj["diffuse"] = (double) pDirLight->GetDiffuse();
            NodeObj["specular"] = (double) pDirLight->GetSpecular();
            NodeObj["radiance"] = (double) pDirLight->GetRadiance();
            NodeObj["direction"] = SWriteVec3(pDirLight->GetDirection());
            NodeObj["enabled"] = pDirLight->GetEnabled();
            SWriteObjectProps(NodeObj, pDirLight);
        }
        else if (auto *pFreeCamera = dynamic_cast<FreeCamera *>(pNode))
        {
            NodeObj["node_type_name"] = "FreeCamera";
            SWriteObjectProps(NodeObj, pFreeCamera);
            SWritePerspCameraProps(NodeObj, pFreeCamera);
        }
        else if (auto *pPersecutor = dynamic_cast<PersecutorCamera *>(pNode))
        {
            NodeObj["node_type_name"] = "PersecutorCamera";
            NodeObj["distance"] = (double) pPersecutor->GetDistance();
            NodeObj["yaw"] = (double) pPersecutor->GetYaw();
            NodeObj["pitch"] = (double) pPersecutor->GetPitch();
            NodeObj["angular_speed"] = (double) pPersecutor->GetAngularSpeed();
            NodeObj["linear_speed"] = (double) pPersecutor->GetLinearSpeed();
            SWriteObjectProps(NodeObj, pPersecutor);
            SWritePerspCameraProps(NodeObj, pPersecutor);
        }
        else if (auto *pDummyCamera = dynamic_cast<DummyCamera *>(pNode))
        {
            NodeObj["node_type_name"] = "DummyCamera";
            SWriteObjectProps(NodeObj, pDummyCamera);
            SWritePerspCameraProps(NodeObj, pDummyCamera);
        }
        else if (auto *pDummyObject = dynamic_cast<DummyObject *>(pNode))
        {
            NodeObj["node_type_name"] = "DummyObject";
            SWriteObjectProps(NodeObj, pDummyObject);
        }
        else if (auto *pModel = dynamic_cast<TexturedModel *>(pNode))
        {
            NodeObj["node_type_name"] = "TexturedModel";
            NodeObj["scene_name"] = pModel->GetModelName();
            NodeObj["color"] = SWriteColor(pModel->GetColor());
            NodeObj["use_color"] = pModel->GetUseColor();
            SWriteObjectProps(NodeObj, pModel);
        }
        else if (auto *pPointLight = dynamic_cast<PointLight *>(pNode))
        {
            NodeObj["node_type_name"] = "PointLight";
            NodeObj["color"] = SWriteColor(pPointLight->GetColor());
            NodeObj["ambient"] = (double) pPointLight->GetAmbient();
            NodeObj["diffuse"] = (double) pPointLight->GetDiffuse();
            NodeObj["specular"] = (double) pPointLight->GetSpecular();
            NodeObj["constant"] = (double) pPointLight->GetConstant();
            NodeObj["linear"] = (double) pPointLight->GetLinear();
            NodeObj["quadratic"] = (double) pPointLight->GetQuadratic();
            NodeObj["enabled"] = pPointLight->GetEnabled();
            SWriteObjectProps(NodeObj, pPointLight);
        }
        else
        {
            // PrimitiveModel and other unsupported types are skipped silently.
            continue;
        }

        // ── Common node fields ─────────────────────────────────────────────────

        NodeObj["node_name"] = QString::fromStdString(pNode->GetNodeName());
        NodeObj["uuid"] = UuidMap[pNode];

        if (pNode->GetParent() != nullptr)
        {
            auto It = UuidMap.find(pNode->GetParent());
            if (It != UuidMap.end())
                NodeObj["parent_uuid"] = It.value();
        }

        NodesArray.append(NodeObj);
    }

    QJsonObject Root;
    Root["nodes"] = NodesArray;

    const QJsonDocument Doc(Root);
    return Util::WriteTextToFile(QString::fromStdString(FilePath), Doc.toJson(QJsonDocument::Indented));
}
