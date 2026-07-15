#include "SceneNode.h"

Canavar::Engine::SceneNode::SceneNode(const QMatrix4x4 &Transformation, const QVector<SceneNodePtr> &Children, const QVector<MeshWeakPtr> &Meshes)
    : mTransformation(Transformation)
    , mChildren(Children)
    , mMeshes(Meshes)
{}

void Canavar::Engine::SceneNode::Render(TexturedModel *pTexturedModel, Shader *pShader, RenderPass RenderPass, const QMatrix4x4 &ParentTransformation)
{
    for (const auto &pMesh : mMeshes)
    {
        if (auto pMeshLocked = pMesh.lock())
        {
            pMeshLocked->Render(pTexturedModel, pShader, RenderPass, ParentTransformation * mTransformation);
        }
    }

    for (const auto &pChild : mChildren)
    {
        pChild->Render(pTexturedModel, pShader, RenderPass, ParentTransformation * mTransformation);
    }
}
