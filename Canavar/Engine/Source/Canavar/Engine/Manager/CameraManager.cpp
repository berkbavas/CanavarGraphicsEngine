#include "CameraManager.h"

#include "Canavar/Engine/Util/Logger.h"

Canavar::Engine::CameraManager::CameraManager(QObject* parent)
    : Manager(parent)
{}

void Canavar::Engine::CameraManager::Initialize()
{
    LOG_DEBUG("CameraManager::Initialize: Initializing...");

    mFreeCamera = std::make_shared<FreeCamera>();
    SetActiveCamera(mFreeCamera);
    LOG_DEBUG("CameraManager::Initialize: Initialization is done.");
}

void Canavar::Engine::CameraManager::Resize(int w, int h)
{
    if (mActiveCamera)
    {
        mActiveCamera->Resize(w, h);
    }
}

void Canavar::Engine::CameraManager::SetDevicePixelRatio(float dpr)
{
    if (mActiveCamera)
    {
        mActiveCamera->SetDevicePixelRatio(dpr);
    }
}

void Canavar::Engine::CameraManager::PreUpdate(float ifps)
{
    if (mActiveCamera)
    {
        mActiveCamera->Update(ifps);
    }
}

void Canavar::Engine::CameraManager::SetActiveCamera(CameraPtr pCamera)
{
    LOG_DEBUG("CameraManager::SetActiveCamera: mActiveCamera: {}, pCamera: {}", PRINT_ADDRESS(mActiveCamera.get()), PRINT_ADDRESS(pCamera.get()));
    CGE_ASSERT(pCamera != nullptr);

    if (mActiveCamera == pCamera)
    {
        return;
    }

    if (mActiveCamera)
    {
        mActiveCamera->Reset();
        pCamera->Resize(mActiveCamera->GetWidth(), mActiveCamera->GetHeight());
    }

    mActiveCamera = pCamera;

    mActiveCamera->Update(0);
}

Canavar::Engine::CameraPtr Canavar::Engine::CameraManager::GetActiveCamera() const
{
    return mActiveCamera;
}

void Canavar::Engine::CameraManager::OnKeyPressed(QKeyEvent* event)
{
    if (mActiveCamera)
    {
        mActiveCamera->KeyPressed(event);
    }
}

void Canavar::Engine::CameraManager::OnKeyReleased(QKeyEvent* event)
{
    if (mActiveCamera)
    {
        mActiveCamera->KeyReleased(event);
    }
}

void Canavar::Engine::CameraManager::OnMousePressed(QMouseEvent* event)
{
    if (mActiveCamera)
    {
        mActiveCamera->MousePressed(event);
    }
}

void Canavar::Engine::CameraManager::OnMouseReleased(QMouseEvent* event)
{
    if (mActiveCamera)
    {
        mActiveCamera->MouseReleased(event);
    }
}

void Canavar::Engine::CameraManager::OnMouseMoved(QMouseEvent* event)
{
    if (mActiveCamera)
    {
        mActiveCamera->MouseMoved(event);
    }
}

void Canavar::Engine::CameraManager::OnWheelMoved(QWheelEvent* event)
{
    if (mActiveCamera)
    {
        mActiveCamera->WheelMoved(event);
    }
}
