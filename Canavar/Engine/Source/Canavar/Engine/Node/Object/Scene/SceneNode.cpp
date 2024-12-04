#include "SceneNode.h"

void Canavar::Engine::SceneNode::Render(Model *pModel, Shader *pShader)
{
    for (const auto &pMesh : mMeshes)
    {
        pMesh->Render(pModel, pShader);
    }

    for (const auto &pChild : mChildren)
    {
        pChild->Render(pModel, pShader);
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
