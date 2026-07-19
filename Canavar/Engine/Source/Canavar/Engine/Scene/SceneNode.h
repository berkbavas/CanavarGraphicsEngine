#pragma once

#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/Object/AABB.h"
#include "Canavar/Engine/Scene/Mesh.h"

#include <memory>

#include <QMatrix4x4>
#include <QVector>

namespace Canavar::Engine
{
    class TexturedModel;
    class SceneNode;
    using SceneNodePtr = std::shared_ptr<SceneNode>;

    class SceneNode
    {
      public:
        explicit SceneNode(const QMatrix4x4 &Transformation, const QVector<SceneNodePtr> &Children, const QVector<MeshWeakPtr> &Meshes);
        SceneNode(const SceneNode &) = delete;
        SceneNode &operator=(const SceneNode &) = delete;
        ~SceneNode() = default;

        const QMatrix4x4 &GetTransformation() const;
        const QVector<SceneNodePtr> &GetChildren() const;
        const QVector<MeshWeakPtr> &GetMeshes() const;
        const AABB &GetBoundingBox() const;

        void Render(TexturedModel *pTexturedModel, Shader *pShader, RenderPass RenderPass, const QMatrix4x4 &ParentTransformation);
        void CalculateBoundingBox(); // Calculate the bounding box for this node based on its meshes and children's bounding boxes

      private:
        QMatrix4x4 mTransformation;
        QVector<SceneNodePtr> mChildren;
        QVector<MeshWeakPtr> mMeshes;
        AABB mBoundingBox; // Axis-Aligned Bounding Box for the scene node
    };
}
