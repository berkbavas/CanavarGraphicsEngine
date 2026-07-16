#include "NodeManager.h"

#include "Canavar/Engine/Manager/Renderer.h"

#include <vector>

Canavar::Engine::NodeManager::NodeManager(Renderer *pRenderer)
    : mRenderer(pRenderer)
{}

void Canavar::Engine::NodeManager::Initialize()
{
    mLightManager = mRenderer->GetLightManager();
}

void Canavar::Engine::NodeManager::RemoveNode(Node *pNode)
{
    // ── Clean up hierarchy links ──────────────────────────────────────────────
    // Detach from parent so the parent's child list has no dangling pointer.
    if (pNode->GetParent())
    {
        pNode->SetParent(nullptr);
    }

    // Orphan direct children so they become root nodes (prevents dangling parent ptrs).
    // Copy the list first — SetParent modifies pNode->mChildren during iteration.
    const std::vector<Node*> ChildrenCopy = pNode->GetChildren();
    for (Node *pChild : ChildrenCopy)
    {
        pChild->SetParent(nullptr);
    }

    // ── Remove from manager lists ─────────────────────────────────────────────
    mNodes.remove(pNode);
    mObjects.remove(dynamic_cast<Object *>(pNode));
    mPointLights.remove(dynamic_cast<PointLight *>(pNode));
    mDirectionalLights.remove(dynamic_cast<DirectionalLight *>(pNode));
    mLightManager->RemoveLight(dynamic_cast<Light *>(pNode));

    mCameras.remove_if([pNode](const CameraPtr &pCamera) { return pCamera.get() == pNode; });
    mLights.remove_if([pNode](const LightPtr &pLight) { return pLight.get() == pNode; });
    mTexturedModels.remove_if([pNode](const TexturedModelPtr &pTexturedModel) { return pTexturedModel.get() == pNode; });
    mPrimitiveModels.remove_if([pNode](const PrimitiveModelPtr &pPrimitiveModel) { return pPrimitiveModel.get() == pNode; });
    mGlobalNodes.remove_if([pNode](const GlobalNodePtr &pGlobalNode) { return pGlobalNode.get() == pNode; });
}

const std::list<Canavar::Engine::CameraPtr> &Canavar::Engine::NodeManager::GetCameras() const
{
    return mCameras;
}
const std::list<Canavar::Engine::LightPtr> &Canavar::Engine::NodeManager::GetLights() const
{
    return mLights;
}

const std::list<Canavar::Engine::TexturedModelPtr> &Canavar::Engine::NodeManager::GetTexturedModels() const
{
    return mTexturedModels;
}

const std::list<Canavar::Engine::PrimitiveModelPtr> &Canavar::Engine::NodeManager::GetPrimitiveModels() const
{
    return mPrimitiveModels;
}

const std::list<Canavar::Engine::Node *> &Canavar::Engine::NodeManager::GetNodes() const
{
    return mNodes;
}

const std::list<Canavar::Engine::Object *> &Canavar::Engine::NodeManager::GetObjects() const
{
    return mObjects;
}

const std::list<Canavar::Engine::PointLight *> &Canavar::Engine::NodeManager::GetPointLights() const
{
    return mPointLights;
}

const std::list<Canavar::Engine::DirectionalLight *> &Canavar::Engine::NodeManager::GetDirectionalLights() const
{
    return mDirectionalLights;
}
