#include "CameraManager.h"

Canavar::Engine::PerspectiveCamera *Canavar::Engine::CameraManager::GetActiveCamera() const
{
    return mActiveCamera;
}

void Canavar::Engine::CameraManager::SetActiveCamera(PerspectiveCamera *pCamera)
{
    if (mActiveCamera)
    {
        mActiveCamera->Reset();
        pCamera->Resize(mActiveCamera->GetWidth(), mActiveCamera->GetHeight());
    }

    mActiveCamera = pCamera;
    mActiveCamera->Reset();
    mActiveCamera->Update(0.0f); // Update the active camera immediately after setting it
}

void Canavar::Engine::CameraManager::Update(float Ifps)
{
    mActiveCamera->Update(Ifps);
}

void Canavar::Engine::CameraManager::Resize(int Width, int Height)
{
    mActiveCamera->Resize(Width, Height);
}

bool Canavar::Engine::CameraManager::OnKeyPressed(QKeyEvent *pEvent)
{
    return mActiveCamera->OnKeyPressed(pEvent);
}

bool Canavar::Engine::CameraManager::OnKeyReleased(QKeyEvent *pEvent)
{
    return mActiveCamera->OnKeyReleased(pEvent);
}

bool Canavar::Engine::CameraManager::OnMousePressed(QMouseEvent *pEvent)
{
    return mActiveCamera->OnMousePressed(pEvent);
}

bool Canavar::Engine::CameraManager::OnMouseReleased(QMouseEvent *pEvent)
{
    return mActiveCamera->OnMouseReleased(pEvent);
}

bool Canavar::Engine::CameraManager::OnMouseMoved(QMouseEvent *pEvent)
{
    return mActiveCamera->OnMouseMoved(pEvent);
}

bool Canavar::Engine::CameraManager::OnWheelMoved(QWheelEvent *pEvent)
{
    return mActiveCamera->OnWheelMoved(pEvent);
}

bool Canavar::Engine::CameraManager::OnLeaveEvent(QEvent *pEvent)
{
    return mActiveCamera->OnLeaveEvent(pEvent);
}
