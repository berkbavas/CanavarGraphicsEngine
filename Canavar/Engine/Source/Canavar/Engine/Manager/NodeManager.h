#pragma once

#include "Canavar/Engine/Manager/Manager.h"
#include "Canavar/Engine/Node/Effects/NozzleEffect/NozzleEffect.h"
#include "Canavar/Engine/Node/Haze/Haze.h"
#include "Canavar/Engine/Node/Light/DirectionalLight.h"
#include "Canavar/Engine/Node/Model/Model.h"
#include "Canavar/Engine/Node/Scene/Scene.h"
#include "Canavar/Engine/Node/Sky/Sky.h"
#include "Canavar/Engine/Node/Terrain/Terrain.h"
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

        ScenePtr GetScene(const QString &sceneName);
        ScenePtr GetScene(ModelPtr pModel);

        const std::map<QString, ScenePtr> &GetScenes() const;
        const std::set<ModelPtr> &GetModels() const;
        const std::set<NodePtr> &GetNodes() const;

      private:
        std::map<QString, ScenePtr> mScenes; // 3D model data

        std::set<ModelPtr> mModels;
        std::set<NodePtr> mNodes;

        LightManager *mLightManager{ nullptr };

        DEFINE_MEMBER_CONST(SkyPtr, Sky);
        DEFINE_MEMBER_CONST(TerrainPtr, Terrain);
        DEFINE_MEMBER_CONST(HazePtr, Haze);
        DEFINE_MEMBER_CONST(DirectionalLightPtr, Sun);

        uint32_t mCurrentNodeId{ 0 };
    };
}