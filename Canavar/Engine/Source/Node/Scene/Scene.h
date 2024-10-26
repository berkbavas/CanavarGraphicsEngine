#pragma once

#include "Node/Scene/SceneNode.h"
#include "Util/Macros.h"
#include "Util/Shader.h"

#include <QVector>

namespace Canavar::Engine
{
    class Scene
    {
      public:
        Scene() = default;

        void Initialize();
        void Destroy();
        void Render(Model *pModel, Shader *pShader);

        void AddMesh(MeshPtr pMesh);
        void AddMaterial(MaterialPtr pMaterial);

        MeshPtr GetMesh(int index);
        MaterialPtr GetMaterial(int index);

      private:
        // Scene class own these meshes and materials.
        QVector<MeshPtr> mMeshes;
        QVector<MaterialPtr> mMaterials;

        DEFINE_MEMBER(SceneNodePtr, RootNode);
        DEFINE_MEMBER(QString, SceneName);
    };

    using ScenePtr = std::shared_ptr<Scene>;

}
