#include "ModelDataNode.h"

#include "ModelData.h"

Canavar::Engine::ModelDataNode::ModelDataNode(ModelData* data)
    : Node()
    , mModelData(data)
{}

void Canavar::Engine::ModelDataNode::AddMeshIndex(int index)
{
    mMeshIndices << index;
}

void Canavar::Engine::ModelDataNode::Render(RenderModes modes, Model* model)
{
    auto meshes = mModelData->GetMeshes();

    for (auto index : qAsConst(mMeshIndices))
        meshes[index]->Render(modes, model);

    for (int i = 0; i < mChildren.size(); i++)
        if (auto child = dynamic_cast<ModelDataNode*>(mChildren[i]))
            child->Render(modes, model);
}