#pragma once

#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/Scene/Mesh.h"
#include "Canavar/Engine/Scene/SceneNode.h"
#include "Canavar/Engine/Scene/TextureMaterial.h"

#include <QString>
#include <QVector>

namespace Canavar::Engine
{
    class Scene
    {
      public:
        explicit Scene(const QString &SceneName, const QVector<MeshPtr> &Meshes, const QVector<TextureMaterialPtr> &Materials, SceneNodePtr RootNode);
        Scene(const Scene &) = delete;
        Scene &operator=(const Scene &) = delete;
        ~Scene() = default;

        void Render(TexturedModel *pTexturedModel, Shader *pShader, RenderPass RenderPass);

        const QString &GetSceneName() const;
        const std::string &GetSceneNameStdString() const;
        SceneNodePtr GetRootNode() const;

      private:
        QString mSceneName;                     // The name of the scene, used for logging and identification.
        std::string mSceneNameStdString;        // Standard string version of the scene name, used for identification.
        QVector<MeshPtr> mMeshes;               // We own all the meshes in this model.
        QVector<TextureMaterialPtr> mMaterials; // We own all the materials in this model.
        SceneNodePtr mRootNode;                 // The root node of the scene's scene graph.
    };

    using ScenePtr = std::shared_ptr<Scene>;
}
