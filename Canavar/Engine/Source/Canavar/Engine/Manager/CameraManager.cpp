#include "CameraManager.h"

#include "Canavar/Engine/Util/Logger.h"

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

void Canavar::Engine::CameraManager::Update(float ifps)
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

Canavar::Engine::PerspectiveCamera* Canavar::Engine::CameraManager::GetActiveCamera() const
{
    return dynamic_cast<PerspectiveCamera*>(mActiveCamera.get());
}

bool Canavar::Engine::CameraManager::KeyPressed(QKeyEvent* event)
{
    if (mActiveCamera)
    {
        return mActiveCamera->KeyPressed(event);
    }

    return false;
}

bool Canavar::Engine::CameraManager::KeyReleased(QKeyEvent* event)
{
    if (mActiveCamera)
    {
        return mActiveCamera->KeyReleased(event);
    }

    return false;
}

bool Canavar::Engine::CameraManager::MousePressed(QMouseEvent* event)
{
    if (mActiveCamera)
    {
        return mActiveCamera->MousePressed(event);
    }

    return false;
}

bool Canavar::Engine::CameraManager::MouseReleased(QMouseEvent* event)
{
    if (mActiveCamera)
    {
        return mActiveCamera->MouseReleased(event);
    }

    return false;
}

bool Canavar::Engine::CameraManager::MouseMoved(QMouseEvent* event)
{
    if (mActiveCamera)
    {
        return mActiveCamera->MouseMoved(event);
    }
    return false;
}

bool Canavar::Engine::CameraManager::WheelMoved(QWheelEvent* event)
{
    if (mActiveCamera)
    {
        return mActiveCamera->WheelMoved(event);
    }

    return false;
}
