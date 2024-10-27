#pragma once

#include "Canavar/Engine/Node/Model/Model.h"
#include "Canavar/Engine/Node/Scene/Mesh.h"
#include "Canavar/Engine/Util/Shader.h"

#include <memory>
#include <set>

namespace Canavar::Engine
{
    class SceneNode;
    using SceneNodePtr = std::shared_ptr<SceneNode>;

    class SceneNode
    {
      public:
        SceneNode() = default;

        void Render(Model *pModel, Shader *pShader);

        void AddMesh(MeshPtr pMesh);
        void AddChild(SceneNodePtr pChild);

      private:
        std::set<SceneNodePtr> mChildren; // Our children.
        std::set<MeshPtr> mMeshes;        // We don't own these meshes; we just store pointers. Owner is Scene.

        DEFINE_MEMBER(QString, NodeName);
    };

}
