#pragma once

#include "Canavar/Engine/Manager/Manager.h"
#include "Canavar/Engine/Node/GlobalNode/Haze/Haze.h"
#include "Canavar/Engine/Node/GlobalNode/Sky/Sky.h"
#include "Canavar/Engine/Node/GlobalNode/Terrain/Terrain.h"
#include "Canavar/Engine/Node/Object/Effect/NozzleEffect/NozzleEffect.h"
#include "Canavar/Engine/Node/Object/Light/DirectionalLight.h"
#include "Canavar/Engine/Node/Object/Model/Model.h"
#include "Canavar/Engine/Node/Object/Scene/Scene.h"
#include "Canavar/Engine/Util/Macros.h"

#include <map>
#include <set>

namespace Canavar::Engine
{
    class LightManager;

    class NodeManager : public Manager
    {
      public:
        explicit NodeManager(QObject *parent = nullptr);

        void Initialize() override;
        void PostInitialize() override;

        void AddNode(NodePtr pNode);
        void RemoveNode(NodePtr pNode);

        ScenePtr GetScene(const QString &sceneName) const;
        ScenePtr GetScene(ModelPtr pModel) const;

        const std::map<QString, ScenePtr> &GetScenes() const;
        const QList<ModelPtr> &GetModels() const;
        const QList<NodePtr> &GetNodes() const;
        const QList<ObjectPtr> &GetObjects() const;

        NodePtr GetNodeById(uint32_t nodeId) const;
        MeshPtr GetMeshById(ModelPtr pModel, uint32_t meshId) const;

      private:
        std::map<QString, ScenePtr> mScenes; // 3D model data

        QList<NodePtr> mNodes;
        QList<ObjectPtr> mObjects;
        QList<ModelPtr> mModels;

        LightManager *mLightManager{ nullptr };

        DEFINE_MEMBER_CONST(SkyPtr, Sky);
        DEFINE_MEMBER_CONST(TerrainPtr, Terrain);
        DEFINE_MEMBER_CONST(HazePtr, Haze);
        DEFINE_MEMBER_CONST(DirectionalLightPtr, Sun);

        uint32_t mCurrentNodeId{ 0 };
    };
}