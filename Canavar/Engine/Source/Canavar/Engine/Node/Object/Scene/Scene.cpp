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

void Canavar::Engine::Scene::Render(Model *pModel, Shader *pShader)
{
    mRootNode->Render(pModel, pShader);
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
    if (0 <= index && index < mMeshes.size())
    {
        return mMeshes[index];
    }
    else
    {
        return nullptr;
    }
}

Canavar::Engine::MaterialPtr Canavar::Engine::Scene::GetMaterial(int index)
{
    if (0 <= index && index < mMaterials.size())
    {
        return mMaterials[index];
    }
    else
    {
        return nullptr;
    }
}
