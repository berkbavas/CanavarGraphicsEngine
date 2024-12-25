#include "SceneNode.h"

void Canavar::Engine::SceneNode::Render(Model *pModel, Shader *pShader, const QMatrix4x4 &Parent4x4)
{
    for (const auto &pMesh : mMeshes)
    {
        pMesh->Render(pModel, pShader, Parent4x4 * mTransformation);
    }

    for (const auto &pChild : mChildren)
    {
        pChild->Render(pModel, pShader, Parent4x4 * mTransformation);
    }
}

void Canavar::Engine::SceneNode::AddMesh(MeshPtr pMesh)
{
    mMeshes.emplace(pMesh);
}

void Canavar::Engine::SceneNode::AddChild(SceneNodePtr pChild)
{
    mChildren.emplace(pChild);
}
