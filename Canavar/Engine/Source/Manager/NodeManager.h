#pragma once

#include "Manager/Manager.h"
#include "Node/Model/Model.h"
#include "Node/Scene/Scene.h"

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

        void AddNode(NodePtr pNode);
        void RemoveNode(NodePtr pNode);

        ScenePtr GetScene(const QString &sceneName);
        ScenePtr GetScene(ModelPtr pModel);

        const std::map<QString, ScenePtr> &GetScenes() const;
        const std::set<ModelPtr> &GetModels() const;

      private:
        std::map<QString, ScenePtr> mScenes; // 3D model data

        std::set<ModelPtr> mModels;
        std::set<NodePtr> mNodes;

        LightManager *mLightManager{ nullptr };
    };
}