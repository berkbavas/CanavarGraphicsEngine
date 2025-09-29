#include "Scene.h"

void Canavar::Engine::Scene::Initialize()
{
    for (const auto &pMesh : mMeshes)
    {
        pMesh->Initialize();
    }
}

void Canavar::Engine::Scene::Destroy()
{
    for (const auto &pMesh : mMeshes)
    {
        pMesh->Destroy();
    }
}

void Canavar::Engine::Scene::Render(Model *pModel, Shader *pShader, RenderPass RenderPass)
{
    mRootNode->Render(pModel, pShader, RenderPass, mRootNode->GetTransformation());
}

void Canavar::Engine::Scene::AddMesh(MeshPtr pMesh)
{
    mMeshes.push_back(pMesh);
}

void Canavar::Engine::Scene::AddMaterial(MaterialPtr pMaterial)
{
    mMaterials.push_back(pMaterial);
}

Canavar::Engine::MeshPtr Canavar::Engine::Scene::GetMesh(int Index)
{
    return mMeshes[Index];
}

Canavar::Engine::MaterialPtr Canavar::Engine::Scene::GetMaterial(int Index)
{
    return mMaterials[Index];
}
