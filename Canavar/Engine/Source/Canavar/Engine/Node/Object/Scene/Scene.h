#pragma once

#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/Node/Object/Scene/SceneNode.h"
#include "Canavar/Engine/Util/Macros.h"

#include <QVector>

namespace Canavar::Engine
{
    class Scene
    {
        DISABLE_COPY(Scene);

      public:
        Scene() = default;

        void Initialize();
        void Destroy();
        void Render(Model *pModel, Shader *pShader, RenderPass renderPass);

        void AddMesh(MeshPtr pMesh);
        void AddMaterial(MaterialPtr pMaterial);

        MeshPtr GetMesh(int Index);
        MaterialPtr GetMaterial(int Index);

        const QVector<MeshPtr> &GetMeshes() const { return mMeshes; }
        const QVector<MaterialPtr> &GetMaterials() const { return mMaterials; }

      private:
        // Scene class own these meshes and materials.
        QVector<MeshPtr> mMeshes;
        QVector<MaterialPtr> mMaterials;

        DEFINE_MEMBER(SceneNodePtr, RootNode);
        DEFINE_MEMBER(std::string, SceneName);
        DEFINE_MEMBER(AABB, AABB);
    };

    using ScenePtr = std::shared_ptr<Scene>;
}
