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
    float minX = std::numeric_limits<float>::infinity();
    float minY = std::numeric_limits<float>::infinity();
    float minZ = std::numeric_limits<float>::infinity();

    float maxX = -std::numeric_limits<float>::infinity();
    float maxY = -std::numeric_limits<float>::infinity();
    float maxZ = -std::numeric_limits<float>::infinity();

    const auto &WorldTransformation = ParentTransformation * mTransformation;

    for (const auto &pMesh : mMeshes)
    {
        const auto aabb = pMesh->GetAABB().Transform(WorldTransformation);
        const auto min = aabb.GetMin();
        const auto max = aabb.GetMax();

        if (minX > min.x())
            minX = min.x();

        if (minY > min.y())
            minY = min.y();

        if (minZ > min.z())
            minZ = min.z();

        if (maxX < max.x())
            maxX = max.x();

        if (maxY < max.y())
            maxY = max.y();

        if (maxZ < max.z())
            maxZ = max.z();
    }

    for (const auto &pChild : mChildren)
    {
        const auto aabb = pChild->CalculateAABB(WorldTransformation);

        const auto min = aabb.GetMin();
        const auto max = aabb.GetMax();

        if (minX > min.x())
            minX = min.x();

        if (minY > min.y())
            minY = min.y();

        if (minZ > min.z())
            minZ = min.z();

        if (maxX < max.x())
            maxX = max.x();

        if (maxY < max.y())
            maxY = max.y();

        if (maxZ < max.z())
            maxZ = max.z();
    }

    return AABB{ QVector3D(minX, minY, minZ), QVector3D(maxX, maxY, maxZ) };
}
