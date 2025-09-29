#include "CameraManager.h"

#include "Canavar/Engine/Util/Logger.h"

void Canavar::Engine::CameraManager::Initialize()
{
    LOG_DEBUG("CameraManager::Initialize: Initializing...");

    mFreeCamera = std::make_shared<FreeCamera>();
    SetActiveCamera(mFreeCamera);
    LOG_DEBUG("CameraManager::Initialize: Initialization is done.");
}

void Canavar::Engine::CameraManager::Resize(int Width, int Height)
{
    if (mActiveCamera)
    {
        mActiveCamera->Resize(Width, Height);
    }
}

void Canavar::Engine::CameraManager::SetDevicePixelRatio(float Ratio)
{
    if (mActiveCamera)
    {
        mActiveCamera->SetDevicePixelRatio(Ratio);
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
    LOG_DEBUG("CameraManager::SetActiveCamera: mActiveCamera: {}, pCamera: {}", static_cast<void*>(mActiveCamera.get()), static_cast<void*>(pCamera.get()));
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

bool Canavar::Engine::CameraManager::KeyPressed(QKeyEvent* pEvent)
{
    if (mActiveCamera)
    {
        return mActiveCamera->KeyPressed(pEvent);
    }

    return false;
}

bool Canavar::Engine::CameraManager::KeyReleased(QKeyEvent* pEvent)
{
    if (mActiveCamera)
    {
        return mActiveCamera->KeyReleased(pEvent);
    }

    return false;
}

bool Canavar::Engine::CameraManager::MousePressed(QMouseEvent* pEvent)
{
    if (mActiveCamera)
    {
        return mActiveCamera->MousePressed(pEvent);
    }

    return false;
}

bool Canavar::Engine::CameraManager::MouseReleased(QMouseEvent* pEvent)
{
    if (mActiveCamera)
    {
        return mActiveCamera->MouseReleased(pEvent);
    }

    return false;
}

bool Canavar::Engine::CameraManager::MouseMoved(QMouseEvent* pEvent)
{
    if (mActiveCamera)
    {
        return mActiveCamera->MouseMoved(pEvent);
    }
    return false;
}

bool Canavar::Engine::CameraManager::WheelMoved(QWheelEvent* pEvent)
{
    if (mActiveCamera)
    {
        return mActiveCamera->WheelMoved(pEvent);
    }

    return false;
}
