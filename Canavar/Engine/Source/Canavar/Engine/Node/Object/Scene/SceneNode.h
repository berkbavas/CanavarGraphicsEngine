#pragma once

#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/Core/Structs.h"
#include "Canavar/Engine/Node/Object/Model/Model.h"
#include "Canavar/Engine/Node/Object/Scene/Mesh.h"

#include <memory>
#include <set>

namespace Canavar::Engine
{
    class SceneNode;
    using SceneNodePtr = std::shared_ptr<SceneNode>;

    class SceneNode
    {
        DISABLE_COPY(SceneNode);

      public:
        SceneNode() = default;

        void Render(Model *pModel, Shader *pShader, RenderPass renderPass, const QMatrix4x4 &ParentTransformation = QMatrix4x4());

        void AddMeshesToListIfHasTransparency(Model *pModel, QVector<TransparentMeshListElement> &List, const QMatrix4x4 &ParentTransformation = QMatrix4x4());

        void AddMesh(MeshPtr pMesh);
        void AddChild(SceneNodePtr pChild);

        AABB CalculateAABB(const QMatrix4x4 &ParentTransformation = QMatrix4x4()) const;

      private:
        std::set<SceneNodePtr> mChildren; // Our children.
        std::set<MeshPtr> mMeshes;        // We don't own these meshes; we just store pointers. Owner is Scene.

        DEFINE_MEMBER(QString, NodeName);
        DEFINE_MEMBER(QMatrix4x4, Transformation);
    };

}
