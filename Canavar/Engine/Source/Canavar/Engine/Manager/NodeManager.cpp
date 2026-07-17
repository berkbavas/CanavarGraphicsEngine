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

void Canavar::Engine::NodeManager::AddCamera(CameraPtr &&pCamera)
{
    // Camera is a unique_ptr, so we need to move it into the list. We also store a raw pointer in mObjects for easy access.
    // Camera : Object : Node
    const auto pRawPtr = pCamera.get();
    mCameras.push_back(std::move(pCamera));
    mObjects.push_back(pRawPtr);
}

void Canavar::Engine::NodeManager::AddLight(LightPtr &&pLight)
{
    // Light is a unique_ptr, so we need to move it into the list. We also store a raw pointer in mObjects for easy access.
    // Light : Object : Node
    const auto pRawPtr = pLight.get();
    mLights.push_back(std::move(pLight));
    mObjects.push_back(pRawPtr);

    // Add the light to the LightManager for uniform management.
    mLightManager->AddLight(pRawPtr);
}

void Canavar::Engine::NodeManager::AddTexturedModel(TexturedModelPtr &&pTexturedModel)
{
    // TexturedModel is a unique_ptr, so we need to move it into the list. We also store a raw pointer in mObjects for easy access.
    // TexturedModel : Object : Node
    const auto pRawPtr = pTexturedModel.get();
    mTexturedModels.push_back(std::move(pTexturedModel));
    mObjects.push_back(pRawPtr);
}

void Canavar::Engine::NodeManager::AddPrimitiveModel(PrimitiveModelPtr &&pPrimitiveModel)
{
    // PrimitiveModel is a unique_ptr, so we need to move it into the list. We also store a raw pointer in mObjects for easy access.
    // PrimitiveModel : Object : Node
    const auto pRawPtr = pPrimitiveModel.get();
    mPrimitiveModels.push_back(std::move(pPrimitiveModel));
    mObjects.push_back(pRawPtr);
}

void Canavar::Engine::NodeManager::AddGlobalNode(GlobalNodePtr &&pGlobalNode)
{
    // GlobalNode is a unique_ptr, so we need to move it into the list. We also store a raw pointer in mObjects for easy access.
    // GlobalNode : Node
    mGlobalNodes.push_back(std::move(pGlobalNode));
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
