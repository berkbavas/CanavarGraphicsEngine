#pragma once

#include "Canavar/Engine/Manager/Manager.h"
#include "Canavar/Engine/Node/Global/Haze/Haze.h"
#include "Canavar/Engine/Node/Global/Sky/Sky.h"
#include "Canavar/Engine/Node/Global/Sun/Sun.h"
#include "Canavar/Engine/Node/Global/Terrain/Terrain.h"
#include "Canavar/Engine/Node/Object/Camera/FreeCamera.h"
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
    class CameraManager;

    class NodeManager : public Manager
    {
      public:
        using Manager::Manager;

        void Initialize() override;
        void PostInitialize() override;

        void AddNode(NodePtr pNode);
        void RemoveNode(NodePtr pNode);
        void RemoveNode(Node* pNode);

        bool HasScene(const std::string &SceneName) const;
        ScenePtr GetScene(const std::string &SceneName) const;
        ScenePtr GetScene(ModelPtr pModel) const;
        ScenePtr GetScene(Model *pModel) const;

        const std::map<std::string, ScenePtr> &GetScenes() const;
        const QList<ModelPtr> &GetModels() const;
        const QList<NodePtr> &GetNodes() const;
        const QList<ObjectPtr> &GetObjects() const;

        MeshPtr GetMeshById(ModelPtr pModel, uint32_t MeshId) const;

        template<typename T = Node>
        std::shared_ptr<T> FindNodeById(uint32_t NodeId) const
        {
            for (const auto &pNode : mNodes)
            {
                if (pNode->GetNodeId() == NodeId)
                {
                    return std::dynamic_pointer_cast<T>(pNode);
                }
            }

            return nullptr;
        }

        template<typename T>
        std::shared_ptr<T> FindNodeByName(const QString &Name)
        {
            for (const auto &pNode : mNodes)
            {
                if (pNode->GetNodeName() == Name)
                {
                    return std::dynamic_pointer_cast<T>(pNode);
                }
            }

            return nullptr;
        }

        void ExportNodes(const QString &Path);
        void ImportNodes(const QString &Path);

        void RemoveAllNodes();

      private:
        std::map<std::string, ScenePtr> mScenes; // 3D model data

        QList<NodePtr> mNodes;
        QList<ObjectPtr> mObjects;
        QList<ModelPtr> mModels;

        LightManager *mLightManager{ nullptr };

        DEFINE_MEMBER_CONST(SkyPtr, Sky);
        DEFINE_MEMBER_CONST(HazePtr, Haze);
        DEFINE_MEMBER_CONST(SunPtr, Sun);
        DEFINE_MEMBER_CONST(FreeCameraPtr, FreeCamera);
        DEFINE_MEMBER_CONST(TerrainPtr, Terrain);

        uint32_t mCurrentNodeId{ 0 };
    };
}