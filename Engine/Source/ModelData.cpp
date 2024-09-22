#include "ModelData.h"

Canavar::Engine::ModelData::ModelData(const QString& name)
    : QObject()
    , mName(name)
    , mRootNode(nullptr)
{}

Canavar::Engine::ModelData::~ModelData()
{
    // TODO
}

void Canavar::Engine::ModelData::AddMesh(Mesh* mesh)
{
    mMeshes << mesh;
}

void Canavar::Engine::ModelData::AddMaterial(Material* material)
{
    mMaterials << material;
}

void Canavar::Engine::ModelData::SetRootNode(ModelDataNode* newRootNode)
{
    mRootNode = newRootNode;
}

Canavar::Engine::Material* Canavar::Engine::ModelData::GetMaterial(int index)
{
    return mMaterials[index];
}

Canavar::Engine::Mesh* Canavar::Engine::ModelData::GetMeshByID(unsigned int id)
{
    for (const auto& mesh : mMeshes)
        if (mesh->GetID() == id)
            return mesh;

    return nullptr;
}

const QString& Canavar::Engine::ModelData::GetName() const
{
    return mName;
}

const QVector<Canavar::Engine::Mesh*>& Canavar::Engine::ModelData::GetMeshes() const
{
    return mMeshes;
}

void Canavar::Engine::ModelData::Render(RenderModes modes, Model* model)
{
    mRootNode->Render(modes, model);
}