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
    // Set Scene's BoundingBox to include the new model's bounding box.

    if (Scene *pScene = mRenderer->GetSceneByName(pTexturedModel->GetModelName()))
    {
        const auto& AABB = pScene->GetRootNode()->GetBoundingBox();
        pTexturedModel->SetAABB(AABB);
    }

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

static void SApplySkyProps(Canavar::Engine::Sky *pSky, const QJsonObject &O)
{
    if (O.contains("enabled"))
        pSky->SetEnabled(O["enabled"].toBool());
    if (O.contains("sun_intensity"))
        pSky->SetSunIntensity((float) O["sun_intensity"].toDouble());
    if (O.contains("planet_radius"))
        pSky->SetPlanetRadius((float) O["planet_radius"].toDouble());
    if (O.contains("atmosphere_radius"))
        pSky->SetAtmosphereRadius((float) O["atmosphere_radius"].toDouble());
    if (O.contains("beta_rayleigh"))
        pSky->SetBetaRayleigh(SReadVec3(O["beta_rayleigh"].toObject()));
    if (O.contains("beta_mie"))
        pSky->SetBetaMie((float) O["beta_mie"].toDouble());
    if (O.contains("scale_height_r"))
        pSky->SetScaleHeightR((float) O["scale_height_r"].toDouble());
    if (O.contains("scale_height_m"))
        pSky->SetScaleHeightM((float) O["scale_height_m"].toDouble());
    if (O.contains("mie_g"))
        pSky->SetMieG((float) O["mie_g"].toDouble());
    if (O.contains("horizon_offset"))
        pSky->SetHorizonOffset((float) O["horizon_offset"].toDouble());
}

static void SApplyHazeProps(Canavar::Engine::Haze *pHaze, const QJsonObject &O)
{
    if (O.contains("color"))
        pHaze->SetColor(SReadColor(O["color"].toObject()));
    if (O.contains("density"))
        pHaze->SetDensity((float) O["density"].toDouble());
    if (O.contains("gradient"))
        pHaze->SetGradient((float) O["gradient"].toDouble());
    if (O.contains("enabled"))
        pHaze->SetEnabled(O["enabled"].toBool());
}

static void SApplyTerrainProps(Canavar::Engine::Terrain *pTerrain, const QJsonObject &O)
{
    if (O.contains("octaves"))
        pTerrain->SetOctaves(O["octaves"].toInt());
    if (O.contains("amplitude"))
        pTerrain->SetAmplitude((float) O["amplitude"].toDouble());
    if (O.contains("frequency"))
        pTerrain->SetFrequency((float) O["frequency"].toDouble());
    if (O.contains("persistence"))
        pTerrain->SetPersistence((float) O["persistence"].toDouble());
    if (O.contains("lacunarity"))
        pTerrain->SetLacunarity((float) O["lacunarity"].toDouble());
    if (O.contains("tessellation_multiplier"))
        pTerrain->SetTessellationMultiplier((float) O["tessellation_multiplier"].toDouble());
    if (O.contains("ambient"))
        pTerrain->SetAmbient((float) O["ambient"].toDouble());
    if (O.contains("diffuse"))
        pTerrain->SetDiffuse((float) O["diffuse"].toDouble());
    if (O.contains("specular"))
        pTerrain->SetSpecular((float) O["specular"].toDouble());
    if (O.contains("shininess"))
        pTerrain->SetShininess((float) O["shininess"].toDouble());
    if (O.contains("enabled"))
        pTerrain->SetEnabled(O["enabled"].toBool());
}

static void SApplyDirectionalLightProps(Canavar::Engine::DirectionalLight *pLight, const QJsonObject &O)
{
    if (O.contains("color"))
        pLight->SetColor(SReadColor(O["color"].toObject()));
    if (O.contains("ambient"))
        pLight->SetAmbient((float) O["ambient"].toDouble());
    if (O.contains("diffuse"))
        pLight->SetDiffuse((float) O["diffuse"].toDouble());
    if (O.contains("specular"))
        pLight->SetSpecular((float) O["specular"].toDouble());
    if (O.contains("radience"))
        pLight->SetRadiance((float) O["radience"].toDouble()); // legacy misspelling
    if (O.contains("radiance"))
        pLight->SetRadiance((float) O["radiance"].toDouble());
    if (O.contains("direction"))
        pLight->SetDirection(SReadVec3(O["direction"].toObject()));
    if (O.contains("enabled"))
        pLight->SetEnabled(O["enabled"].toBool());
}

static void SApplyPointLightProps(Canavar::Engine::PointLight *pLight, const QJsonObject &O)
{
    if (O.contains("color"))
        pLight->SetColor(SReadColor(O["color"].toObject()));
    if (O.contains("ambient"))
        pLight->SetAmbient((float) O["ambient"].toDouble());
    if (O.contains("diffuse"))
        pLight->SetDiffuse((float) O["diffuse"].toDouble());
    if (O.contains("specular"))
        pLight->SetSpecular((float) O["specular"].toDouble());
    if (O.contains("constant"))
        pLight->SetConstant((float) O["constant"].toDouble());
    if (O.contains("linear"))
        pLight->SetLinear((float) O["linear"].toDouble());
    if (O.contains("quadratic"))
        pLight->SetQuadratic((float) O["quadratic"].toDouble());
    if (O.contains("enabled"))
        pLight->SetEnabled(O["enabled"].toBool());
}

static void SApplyPersecutorCameraProps(Canavar::Engine::PersecutorCamera *pCam, const QJsonObject &O)
{
    SApplyObjectProps(pCam, O);
    SApplyCameraProps(pCam, O);
    if (O.contains("distance"))
        pCam->SetDistance((float) O["distance"].toDouble());
    if (O.contains("yaw"))
        pCam->SetYaw((float) O["yaw"].toDouble());
    if (O.contains("pitch"))
        pCam->SetPitch((float) O["pitch"].toDouble());
    if (O.contains("angular_speed"))
        pCam->SetAngularSpeed((float) O["angular_speed"].toDouble());
    if (O.contains("linear_speed"))
        pCam->SetLinearSpeed((float) O["linear_speed"].toDouble());
}

static void SApplyTexturedModelProps(Canavar::Engine::TexturedModel *pModel, const QJsonObject &O)
{
    if (O.contains("color"))
        pModel->SetColor(SReadColor(O["color"].toObject()));
    if (O.contains("use_color"))
        pModel->SetUseColor(O["use_color"].toBool());
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

    struct PendingChild
    {
        Node *pNode;
        QString ParentUuid;
    };
    QVector<PendingChild> PendingChildren;

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
            Sky *pSky = FindNodeByType<Sky>();
            if (pSky)
                SApplySkyProps(pSky, NodeObj);
            pNode = pSky;
        }
        else if (TypeName == "Haze")
        {
            Haze *pHaze = FindNodeByType<Haze>();
            if (pHaze)
                SApplyHazeProps(pHaze, NodeObj);
            pNode = pHaze;
        }
        else if (TypeName == "Sun")
        {
            DirectionalLight *pSun = mRenderer->GetSun();
            if (pSun)
            {
                SApplyDirectionalLightProps(pSun, NodeObj);
                SApplyObjectProps(pSun, NodeObj);
            }
            pNode = pSun;
        }
        else if (TypeName == "Terrain")
        {
            Terrain *pTerrain = FindNodeByType<Terrain>();
            if (pTerrain)
                SApplyTerrainProps(pTerrain, NodeObj);
            pNode = pTerrain;
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
            SApplyPersecutorCameraProps(pCam, NodeObj);
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
            SApplyTexturedModelProps(pModel, NodeObj);
            pNode = pModel;
        }
        else if (TypeName == "DirectionalLight")
        {
            DirectionalLight *pLight = CreateNode<DirectionalLight>();
            SApplyObjectProps(pLight, NodeObj);
            SApplyDirectionalLightProps(pLight, NodeObj);
            pNode = pLight;
        }
        else if (TypeName == "PointLight")
        {
            PointLight *pLight = CreateNode<PointLight>();
            SApplyObjectProps(pLight, NodeObj);
            SApplyPointLightProps(pLight, NodeObj);
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

        // Set the node's name and UUID if provided
        if (!NodeName.isEmpty())
            pNode->SetNodeName(NodeName.toStdString());

        if (!UuidStr.isEmpty())
            UuidToNode[UuidStr] = pNode;

        // If the node has a parent UUID, store it for later processing
        if (NodeObj.contains("parent_uuid"))
            PendingChildren.push_back({ pNode, NodeObj["parent_uuid"].toString() });
    }

    // Second pass: set parent-child relationships
    for (const auto &PC : PendingChildren)
    {
        auto It = UuidToNode.find(PC.ParentUuid);
        if (It != UuidToNode.end())
            PC.pNode->SetParent(It.value());
        else
            LOG_WARN("NodeManager::ImportNodes: Parent UUID '{}' not found for node '{}'.", PC.ParentUuid.toStdString(), PC.pNode->GetNodeName());
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

static QJsonObject SWritePersecutorCameraProps(const Canavar::Engine::PersecutorCamera *pCam)
{
    QJsonObject O;
    O["node_type_name"] = "PersecutorCamera";
    O["distance"] = (double) pCam->GetDistance();
    O["yaw"] = (double) pCam->GetYaw();
    O["pitch"] = (double) pCam->GetPitch();
    O["angular_speed"] = (double) pCam->GetAngularSpeed();
    O["linear_speed"] = (double) pCam->GetLinearSpeed();
    SWriteObjectProps(O, pCam);
    SWritePerspCameraProps(O, pCam);
    return O;
}

static QJsonObject SWriteDummyCameraProps(const Canavar::Engine::DummyCamera *pCam)
{
    QJsonObject O;
    O["node_type_name"] = "DummyCamera";
    SWriteObjectProps(O, pCam);
    SWritePerspCameraProps(O, pCam);
    return O;
}

static QJsonObject SWriteFreeCameraProps(const Canavar::Engine::FreeCamera *pCam)
{
    QJsonObject O;
    O["node_type_name"] = "FreeCamera";
    SWriteObjectProps(O, pCam);
    SWritePerspCameraProps(O, pCam);
    return O;
}

static QJsonObject SWriteSkyProps(const Canavar::Engine::Sky *pSky)
{
    QJsonObject O;
    O["node_type_name"] = "Sky";
    O["enabled"] = pSky->GetEnabled();
    O["sun_intensity"] = (double) pSky->GetSunIntensity();
    O["planet_radius"] = (double) pSky->GetPlanetRadius();
    O["atmosphere_radius"] = (double) pSky->GetAtmosphereRadius();
    O["beta_rayleigh"] = SWriteVec3(pSky->GetBetaRayleigh());
    O["beta_mie"] = (double) pSky->GetBetaMie();
    O["scale_height_r"] = (double) pSky->GetScaleHeightR();
    O["scale_height_m"] = (double) pSky->GetScaleHeightM();
    O["mie_g"] = (double) pSky->GetMieG();
    O["horizon_offset"] = (double) pSky->GetHorizonOffset();
    return O;
}

static QJsonObject SWriteHazeProps(const Canavar::Engine::Haze *pHaze)
{
    QJsonObject O;
    O["node_type_name"] = "Haze";
    O["color"] = SWriteColor(pHaze->GetColor());
    O["density"] = (double) pHaze->GetDensity();
    O["gradient"] = (double) pHaze->GetGradient();
    O["enabled"] = pHaze->GetEnabled();
    return O;
}

static QJsonObject SWriteTerrainProps(const Canavar::Engine::Terrain *pTerrain)
{
    QJsonObject O;
    O["node_type_name"] = "Terrain";
    O["octaves"] = pTerrain->GetOctaves();
    O["amplitude"] = (double) pTerrain->GetAmplitude();
    O["frequency"] = (double) pTerrain->GetFrequency();
    O["persistence"] = (double) pTerrain->GetPersistence();
    O["lacunarity"] = (double) pTerrain->GetLacunarity();
    O["tessellation_multiplier"] = (double) pTerrain->GetTessellationMultiplier();
    O["ambient"] = (double) pTerrain->GetAmbient();
    O["diffuse"] = (double) pTerrain->GetDiffuse();
    O["specular"] = (double) pTerrain->GetSpecular();
    O["shininess"] = (double) pTerrain->GetShininess();
    O["enabled"] = pTerrain->GetEnabled();
    return O;
}

static QJsonObject SWriteDirectionalLightProps(const Canavar::Engine::DirectionalLight *pDirLight, bool IsSun)
{
    QJsonObject O;
    O["node_type_name"] = IsSun ? "Sun" : "DirectionalLight";
    O["color"] = SWriteColor(pDirLight->GetColor());
    O["ambient"] = (double) pDirLight->GetAmbient();
    O["diffuse"] = (double) pDirLight->GetDiffuse();
    O["specular"] = (double) pDirLight->GetSpecular();
    O["radiance"] = (double) pDirLight->GetRadiance();
    O["direction"] = SWriteVec3(pDirLight->GetDirection());
    O["enabled"] = pDirLight->GetEnabled();
    SWriteObjectProps(O, pDirLight);
    return O;
}

static QJsonObject SWritePointLightProps(const Canavar::Engine::PointLight *pPointLight)
{
    QJsonObject O;
    O["node_type_name"] = "PointLight";
    O["color"] = SWriteColor(pPointLight->GetColor());
    O["ambient"] = (double) pPointLight->GetAmbient();
    O["diffuse"] = (double) pPointLight->GetDiffuse();
    O["specular"] = (double) pPointLight->GetSpecular();
    O["constant"] = (double) pPointLight->GetConstant();
    O["linear"] = (double) pPointLight->GetLinear();
    O["quadratic"] = (double) pPointLight->GetQuadratic();
    O["enabled"] = pPointLight->GetEnabled();
    SWriteObjectProps(O, pPointLight);
    return O;
}

static QJsonObject SWriteTexturedModelProps(const Canavar::Engine::TexturedModel *pModel)
{
    QJsonObject O;
    O["node_type_name"] = "TexturedModel";
    O["scene_name"] = pModel->GetModelName();
    O["color"] = SWriteColor(pModel->GetColor());
    O["use_color"] = pModel->GetUseColor();
    SWriteObjectProps(O, pModel);
    return O;
}

static QJsonObject SWriteDummyObjectProps(const Canavar::Engine::DummyObject *pDummy)
{
    QJsonObject O;
    O["node_type_name"] = "DummyObject";
    SWriteObjectProps(O, pDummy);
    return O;
}

static void SWriteCommonProps(QJsonObject &O, const Canavar::Engine::Node *pNode, const QMap<const Canavar::Engine::Node *, QString> &UuidMap)
{
    // Add common properties
    O["node_name"] = QString::fromStdString(pNode->GetNodeName());
    O["uuid"] = UuidMap[pNode];

    // Write parent UUID if the node has a parent
    if (pNode->GetParent() != nullptr)
    {
        auto It = UuidMap.find(pNode->GetParent());
        if (It != UuidMap.end())
            O["parent_uuid"] = It.value();
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// ExportNodes
// ─────────────────────────────────────────────────────────────────────────────

bool Canavar::Engine::NodeManager::ExportNodes(const std::string &FilePath)
{
    // Fill a map of Node pointers to their UUID strings for easy lookup when writing parent UUIDs.
    QMap<const Node *, QString> UuidMap;
    for (const auto &pNodePtr : mNodes)
    {
        const Node *pRawNode = pNodePtr.get();
        UuidMap[pRawNode] = pRawNode->GetUuid().toString(QUuid::WithoutBraces);
    }

    QJsonArray NodesArray;

    for (const auto &pNodePtr : mNodes)
    {
        Node *pNode = pNodePtr.get();
        QJsonObject NodeObj;

        // Determine the type of node and write its properties accordingly.
        if (auto *pSky = dynamic_cast<Sky *>(pNode))
            NodeObj = SWriteSkyProps(pSky);
        else if (auto *pHaze = dynamic_cast<Haze *>(pNode))
            NodeObj = SWriteHazeProps(pHaze);
        else if (auto *pTerrain = dynamic_cast<Terrain *>(pNode))
            NodeObj = SWriteTerrainProps(pTerrain);
        else if (auto *pDirLight = dynamic_cast<DirectionalLight *>(pNode))
            NodeObj = SWriteDirectionalLightProps(pDirLight, pDirLight == mRenderer->GetSun());
        else if (auto *pPointLight = dynamic_cast<PointLight *>(pNode))
            NodeObj = SWritePointLightProps(pPointLight);
        else if (auto *pFreeCamera = dynamic_cast<FreeCamera *>(pNode))
            NodeObj = SWriteFreeCameraProps(pFreeCamera);
        else if (auto *pPersecutor = dynamic_cast<PersecutorCamera *>(pNode))
            NodeObj = SWritePersecutorCameraProps(pPersecutor);
        else if (auto *pDummyCamera = dynamic_cast<DummyCamera *>(pNode))
            NodeObj = SWriteDummyCameraProps(pDummyCamera);
        else if (auto *pDummyObject = dynamic_cast<DummyObject *>(pNode))
            NodeObj = SWriteDummyObjectProps(pDummyObject);
        else if (auto *pModel = dynamic_cast<TexturedModel *>(pNode))
            NodeObj = SWriteTexturedModelProps(pModel);
        else if (auto *pPrimitiveModel = dynamic_cast<PrimitiveModel *>(pNode))
        {
            LOG_WARN("NodeManager::ExportNodes: PrimitiveModel '{}' is not supported for export, skipping.", pPrimitiveModel->GetNodeName());
            continue;
        }
        else
        {
            LOG_WARN("NodeManager::ExportNodes: Unsupported node type '{}', skipping.", typeid(*pNode).name());
            continue;
        }

        // Add common properties like node name, UUID, and parent UUID
        SWriteCommonProps(NodeObj, pNode, UuidMap);

        // Add the node object to the array
        NodesArray.append(NodeObj);
    }

    // Write the nodes array to a JSON file
    QJsonObject Root;
    Root["nodes"] = NodesArray;

    const QJsonDocument Doc(Root);
    return Util::WriteTextToFile(QString::fromStdString(FilePath), Doc.toJson(QJsonDocument::Indented));
}
