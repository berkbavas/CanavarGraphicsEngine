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
    const std::vector<Node *> ChildrenCopy = pNode->GetChildren();
    for (Node *pChild : ChildrenCopy)
    {
        pChild->SetParent(nullptr);
    }

    // ── Remove from manager lists ─────────────────────────────────────────────
    mObjects.remove(dynamic_cast<Object *>(pNode));
    mPointLights.remove(dynamic_cast<PointLight *>(pNode));
    mDirectionalLights.remove(dynamic_cast<DirectionalLight *>(pNode));
    mLightManager->RemoveLight(dynamic_cast<Light *>(pNode));
    mCameras.remove(dynamic_cast<Camera *>(pNode));
    mLights.remove(dynamic_cast<Light *>(pNode));
    mTexturedModels.remove(dynamic_cast<TexturedModel *>(pNode));
    mPrimitiveModels.remove(dynamic_cast<PrimitiveModel *>(pNode));
    mGlobalNodes.remove(dynamic_cast<GlobalNode *>(pNode));
    mDummyObjects.remove(dynamic_cast<DummyObject *>(pNode));

    // Remove the node from the main list of nodes. This will also delete the node since we are using unique_ptr.
    mNodes.remove_if([pNode](const NodePtr &ptr) { return ptr.get() == pNode; });
}

void Canavar::Engine::NodeManager::AddCamera(Camera *pCamera)
{
    mCameras.push_back(pCamera);
    mObjects.push_back(pCamera);
}

void Canavar::Engine::NodeManager::AddLight(Light *pLight)
{
    mLights.push_back(pLight);
    mObjects.push_back(pLight);

    // Add the light to the LightManager for uniform management.
    mLightManager->AddLight(pLight);
}

void Canavar::Engine::NodeManager::AddTexturedModel(TexturedModel *pTexturedModel)
{
    mTexturedModels.push_back(pTexturedModel);
    mObjects.push_back(pTexturedModel);
}

void Canavar::Engine::NodeManager::AddPrimitiveModel(PrimitiveModel *pPrimitiveModel)
{
    mPrimitiveModels.push_back(pPrimitiveModel);
    mObjects.push_back(pPrimitiveModel);
}

void Canavar::Engine::NodeManager::AddGlobalNode(GlobalNode *pGlobalNode)
{
    mGlobalNodes.push_back(pGlobalNode);
}

void Canavar::Engine::NodeManager::AddDummyObject(DummyObject *pDummyObject)
{
    mDummyObjects.push_back(pDummyObject);
    mObjects.push_back(pDummyObject);
}

const std::list<Canavar::Engine::NodePtr> &Canavar::Engine::NodeManager::GetNodes() const
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

const std::list<Canavar::Engine::Camera *> &Canavar::Engine::NodeManager::GetCameras() const
{
    return mCameras;
}
const std::list<Canavar::Engine::Light *> &Canavar::Engine::NodeManager::GetLights() const
{
    return mLights;
}

const std::list<Canavar::Engine::TexturedModel *> &Canavar::Engine::NodeManager::GetTexturedModels() const
{
    return mTexturedModels;
}

const std::list<Canavar::Engine::PrimitiveModel *> &Canavar::Engine::NodeManager::GetPrimitiveModels() const
{
    return mPrimitiveModels;
}

const std::list<Canavar::Engine::GlobalNode *> &Canavar::Engine::NodeManager::GetGlobalNodes() const
{
    return mGlobalNodes;
}

const std::list<Canavar::Engine::DummyObject *> &Canavar::Engine::NodeManager::GetDummyObjects() const
{
    return mDummyObjects;
}
