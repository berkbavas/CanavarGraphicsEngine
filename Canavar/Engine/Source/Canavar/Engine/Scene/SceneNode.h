#pragma once

#include "Canavar/Engine/Core/Shader.h"
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

        void Render(TexturedModel *pTexturedModel, Shader *pShader, RenderPass RenderPass, const QMatrix4x4 &ParentTransformation);

      private:
        QMatrix4x4 mTransformation;
        QVector<SceneNodePtr> mChildren;
        QVector<MeshWeakPtr> mMeshes;
    };
}
