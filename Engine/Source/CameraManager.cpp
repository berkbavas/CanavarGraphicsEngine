#include "CameraManager.h"
#include "NodeManager.h"
#include <QtMath>

Canavar::Engine::CameraManager::CameraManager()
    : Manager()
    , mDefaultCamera(nullptr)
    , mActiveCamera(nullptr)
{}

Canavar::Engine::CameraManager* Canavar::Engine::CameraManager::Instance()
{
    static CameraManager instance;
    return &instance;
}

bool Canavar::Engine::CameraManager::Init()
{
    return true;
}

void Canavar::Engine::CameraManager::PostInit()
{
    mDefaultCamera = dynamic_cast<FreeCamera*>(NodeManager::Instance()->CreateNode(Node::NodeType::FreeCamera, "Default Free Camera"));
    mDefaultCamera->SetExcludeFromExport(true);
    SetActiveCamera(mDefaultCamera);
}

void Canavar::Engine::CameraManager::AddCamera(Camera* camera)
{
    mCameras << camera;
}

void Canavar::Engine::CameraManager::RemoveCamera(Camera* camera)
{
    if (camera)
    {
        if (camera == mActiveCamera)
            SetActiveCamera(nullptr);

        mCameras.removeAll(camera);
    }
}

Canavar::Engine::Camera* Canavar::Engine::CameraManager::GetActiveCamera() const
{
    return mActiveCamera;
}

void Canavar::Engine::CameraManager::SetActiveCamera(Camera* newActiveCamera)
{
    if (mActiveCamera == newActiveCamera)
        return;

    if (mActiveCamera)
        mActiveCamera->SetActive(false);

    if (newActiveCamera)
    {
        newActiveCamera->SetActive(true);
        newActiveCamera->SetWidth(mWidth);
        newActiveCamera->SetHeight(mHeight);
    }

    mActiveCamera = newActiveCamera;

    if (!mActiveCamera)
        SetActiveCamera(mDefaultCamera);
}

void Canavar::Engine::CameraManager::MouseDoubleClicked(QMouseEvent* event)
{
    mActiveCamera->MouseDoubleClicked(event);
}

void Canavar::Engine::CameraManager::MousePressed(QMouseEvent* event)
{
    mActiveCamera->MousePressed(event);
}

void Canavar::Engine::CameraManager::MouseReleased(QMouseEvent* event)
{
    mActiveCamera->MouseReleased(event);
}

void Canavar::Engine::CameraManager::MouseMoved(QMouseEvent* event)
{
    mActiveCamera->MouseMoved(event);
}

void Canavar::Engine::CameraManager::WheelMoved(QWheelEvent* event)
{
    mActiveCamera->WheelMoved(event);
}

void Canavar::Engine::CameraManager::KeyPressed(QKeyEvent* event)
{
    mActiveCamera->KeyPressed(event);
}

void Canavar::Engine::CameraManager::KeyReleased(QKeyEvent* event)
{
    mActiveCamera->KeyReleased(event);
}

void Canavar::Engine::CameraManager::Resize(int width, int height)
{
    mWidth = width;
    mHeight = height;

    mActiveCamera->SetWidth(mWidth);
    mActiveCamera->SetHeight(mHeight);
}

void Canavar::Engine::CameraManager::Update(float ifps)
{
    mActiveCamera->Update(ifps);
}

void Canavar::Engine::CameraManager::Reset()
{
    mActiveCamera->Reset();
}