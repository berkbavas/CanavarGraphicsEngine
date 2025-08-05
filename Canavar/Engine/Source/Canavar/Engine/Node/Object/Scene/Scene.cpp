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

void Canavar::Engine::Scene::Render(Model *pModel, Shader *pShader, RenderPass renderPass)
{
    mRootNode->Render(pModel, pShader, renderPass, mRootNode->GetTransformation());
}

void Canavar::Engine::Scene::AddMesh(MeshPtr pMesh)
{
    mMeshes.emplace_back(pMesh);
}

void Canavar::Engine::Scene::AddMaterial(MaterialPtr pMaterial)
{
    mMaterials.emplace_back(pMaterial);
}

Canavar::Engine::MeshPtr Canavar::Engine::Scene::GetMesh(int index)
{
    return mMeshes[index];
}

Canavar::Engine::MaterialPtr Canavar::Engine::Scene::GetMaterial(int index)
{
    return mMaterials[index];
}
