#include "CameraManager.h"

#include "Canavar/Engine/Util/Logger.h"

Canavar::Engine::CameraManager::CameraManager(QObject* parent)
    : Manager(parent)
{}

void Canavar::Engine::CameraManager::Initialize()
{
    LOG_DEBUG("CameraManager::Initialize: Initializing...");

    FreeCameraPtr pFreeCamera = std::make_shared<FreeCamera>();
    pFreeCamera->SetZFar(1'000'000.0f);
    pFreeCamera->SetPosition(0, 0, -3);

    SetActiveCamera(pFreeCamera);
    LOG_DEBUG("CameraManager::Initialize: Initialization is done.");
}

void Canavar::Engine::CameraManager::Resize(int w, int h)
{
    if (mActiveCamera)
    {
        mActiveCamera->Resize(w, h);
    }
}

void Canavar::Engine::CameraManager::PreUpdate(float ifps)
{
    if (mActiveCamera)
    {
        mActiveCamera->Update(ifps);
    }
}

void Canavar::Engine::CameraManager::SetActiveCamera(CameraPtr camera)
{
    LOG_DEBUG("CameraManager::SetActiveCamera: mActiveCamera: {}, camera: {}", PRINT_ADDRESS(camera.get()), PRINT_ADDRESS(mActiveCamera.get()));
    CGE_ASSERT(camera != nullptr);

    if (mActiveCamera)
    {
        camera->Resize(mActiveCamera->GetWidth(), mActiveCamera->GetHeight());
    }

    camera->Update(0);

    mActiveCamera = camera;
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
