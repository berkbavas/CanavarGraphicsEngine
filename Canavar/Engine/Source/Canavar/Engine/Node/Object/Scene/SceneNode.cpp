#include "SceneNode.h"

void Canavar::Engine::SceneNode::Render(Model *pModel, Shader *pShader, RenderPass RenderPass, const QMatrix4x4 &ParentTransformation)
{
    for (const auto &pMesh : mMeshes)
    {
        pMesh->Render(pModel, pShader, (ParentTransformation * mTransformation), RenderPass);
    }

    for (const auto &pChild : mChildren)
    {
        pChild->Render(pModel, pShader, RenderPass, (ParentTransformation * mTransformation));
    }
}

void Canavar::Engine::SceneNode::AddMeshesToListIfHasTransparency(Model *pModel, QVector<TransparentMeshListElement> &List, const QMatrix4x4 &ParentTransformation)
{
    for (const auto &pMesh : mMeshes)
    {
        const auto &ModelMatrix = pModel->GetWorldTransformation();
        const auto MeshMatrix = pModel->GetMeshTransformation(pMesh->GetUniqueMeshName());
        const auto NodeMatrix = ParentTransformation * mTransformation;
        const auto CombinedTranformation = ModelMatrix * (MeshMatrix * NodeMatrix);

        // If the mesh has transparency, add it to the list.
        if (pMesh->HasTransparency(pModel))
        {
            List.push_back({ pModel, pMesh.get(), NodeMatrix, CombinedTranformation });
        }
    }

    for (const auto &pChild : mChildren)
    {
        pChild->AddMeshesToListIfHasTransparency(pModel, List, (ParentTransformation * mTransformation));
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

Canavar::Engine::AABB Canavar::Engine::SceneNode::CalculateAABB(const QMatrix4x4 &ParentTransformation) const
{
    // AABB
    float MinXValue = std::numeric_limits<float>::infinity();
    float MinYValue = std::numeric_limits<float>::infinity();
    float MinZValue = std::numeric_limits<float>::infinity();

    float MaxXValue = -std::numeric_limits<float>::infinity();
    float MaxYValue = -std::numeric_limits<float>::infinity();
    float MaxZValue = -std::numeric_limits<float>::infinity();

    const auto &WorldTransformation = ParentTransformation * mTransformation;

    for (const auto &pMesh : mMeshes)
    {
        const auto AABB = pMesh->GetAABB().Transform(WorldTransformation);
        const auto Min = AABB.GetMin();
        const auto Max = AABB.GetMax();

        if (MinXValue > Min.x())
            MinXValue = Min.x();

        if (MinYValue > Min.y())
            MinYValue = Min.y();

        if (MinZValue > Min.z())
            MinZValue = Min.z();

        if (MaxXValue < Max.x())
            MaxXValue = Max.x();

        if (MaxYValue < Max.y())
            MaxYValue = Max.y();

        if (MaxZValue < Max.z())
            MaxZValue = Max.z();
    }

    for (const auto &pChild : mChildren)
    {
        const auto AABB = pChild->CalculateAABB(WorldTransformation);

        const auto Min = AABB.GetMin();
        const auto Max = AABB.GetMax();

        if (MinXValue > Min.x())
            MinXValue = Min.x();

        if (MinYValue > Min.y())
            MinYValue = Min.y();

        if (MinZValue > Min.z())
            MinZValue = Min.z();

        if (MaxXValue < Max.x())
            MaxXValue = Max.x();

        if (MaxYValue < Max.y())
            MaxYValue = Max.y();

        if (MaxZValue < Max.z())
            MaxZValue = Max.z();
    }

    return AABB{ QVector3D(MinXValue, MinYValue, MinZValue), QVector3D(MaxXValue, MaxYValue, MaxZValue) };
}
