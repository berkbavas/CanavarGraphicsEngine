#include "SceneNode.h"

Canavar::Engine::SceneNode::SceneNode(const QMatrix4x4 &Transformation, const QVector<SceneNodePtr> &Children, const QVector<MeshWeakPtr> &Meshes)
    : mTransformation(Transformation)
    , mChildren(Children)
    , mMeshes(Meshes)
{
    CalculateBoundingBox();
}

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

const QMatrix4x4 &Canavar::Engine::SceneNode::GetTransformation() const
{
    return mTransformation;
}

const QVector<Canavar::Engine::SceneNodePtr> &Canavar::Engine::SceneNode::GetChildren() const
{
    return mChildren;
}

const QVector<Canavar::Engine::MeshWeakPtr> &Canavar::Engine::SceneNode::GetMeshes() const
{
    return mMeshes;
}

const Canavar::Engine::AABB &Canavar::Engine::SceneNode::GetBoundingBox() const
{
    return mBoundingBox;
}

void Canavar::Engine::SceneNode::CalculateBoundingBox()
{
    // Calculate the bounding box for this node based on its meshes and children's bounding boxes
    QVector3D MinPoint(FLT_MAX, FLT_MAX, FLT_MAX);
    QVector3D MaxPoint(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    for (const auto &pMesh : mMeshes)
    {
        if (auto pMeshLocked = pMesh.lock())
        {
            const AABB &MeshBoundingBox = pMeshLocked->GetBoundingBox();
            MinPoint.setX(qMin(MinPoint.x(), MeshBoundingBox.GetMin().x()));
            MinPoint.setY(qMin(MinPoint.y(), MeshBoundingBox.GetMin().y()));
            MinPoint.setZ(qMin(MinPoint.z(), MeshBoundingBox.GetMin().z()));
            MaxPoint.setX(qMax(MaxPoint.x(), MeshBoundingBox.GetMax().x()));
            MaxPoint.setY(qMax(MaxPoint.y(), MeshBoundingBox.GetMax().y()));
            MaxPoint.setZ(qMax(MaxPoint.z(), MeshBoundingBox.GetMax().z()));
        }
    }

    for (const auto &pChild : mChildren)
    {
        const AABB &ChildBoundingBox = pChild->GetBoundingBox();
        MinPoint.setX(qMin(MinPoint.x(), ChildBoundingBox.GetMin().x()));
        MinPoint.setY(qMin(MinPoint.y(), ChildBoundingBox.GetMin().y()));
        MinPoint.setZ(qMin(MinPoint.z(), ChildBoundingBox.GetMin().z()));
        MaxPoint.setX(qMax(MaxPoint.x(), ChildBoundingBox.GetMax().x()));
        MaxPoint.setY(qMax(MaxPoint.y(), ChildBoundingBox.GetMax().y()));
        MaxPoint.setZ(qMax(MaxPoint.z(), ChildBoundingBox.GetMax().z()));
    }

    mBoundingBox = AABB(MinPoint, MaxPoint).Transform(mTransformation);
}
