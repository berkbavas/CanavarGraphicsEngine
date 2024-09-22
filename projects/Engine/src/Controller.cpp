#include "Controller.h"
#include "CameraManager.h"
#include "IntersectionManager.h"
#include "LightManager.h"
#include "ModelDataManager.h"
#include "NodeManager.h"
#include "RendererManager.h"
#include "SelectableNodeRenderer.h"
#include "ShaderManager.h"

Canavar::Engine::Controller::Controller(QObject* parent)
    : QObject(parent)
    , mWindow(nullptr)
    , mSuccess(true)
{}

bool Canavar::Engine::Controller::Init(const QString& configFile)
{
    initializeOpenGLFunctions();

    mConfig = Config::Instance();
    mConfig->Load(configFile);

    mModelDataManager = ModelDataManager::Instance();
    mShaderManager = ShaderManager::Instance();
    mCameraManager = CameraManager::Instance();
    mLightManager = LightManager::Instance();
    mNodeManager = NodeManager::Instance();
    mRendererManager = RendererManager::Instance();
    mSelectableNodeRenderer = SelectableNodeRenderer::Instance();
    mIntersectionManager = IntersectionManager::Instance();

    mManagers << mModelDataManager;
    mManagers << mShaderManager;
    mManagers << mCameraManager;
    mManagers << mLightManager;
    mManagers << mNodeManager;
    mManagers << mSelectableNodeRenderer;
    mManagers << mRendererManager;
    mManagers << mIntersectionManager;

    for (const auto& manager : qAsConst(mManagers))
        if (!manager->Init())
        {
            mSuccess = false;
            break;
        }

    if (mSuccess)
        for (const auto& manager : qAsConst(mManagers))
            manager->PostInit();

    return mSuccess;
}

void Canavar::Engine::Controller::Render(float ifps)
{
    if (!mSuccess)
        return;

    if (!mWindow)
        return;

    for (const auto& manager : qAsConst(mManagers))
        manager->Update(ifps);

    for (const auto& manager : qAsConst(mManagers))
        manager->Render(ifps);
}

void Canavar::Engine::Controller::SetWindow(QOpenGLWindow* newWindow)
{
    mWindow = newWindow;
}

void Canavar::Engine::Controller::MouseDoubleClicked(QMouseEvent* event)
{
    if (!mSuccess)
        return;

    for (const auto& manager : qAsConst(mManagers))
        manager->MouseDoubleClicked(event);
}

void Canavar::Engine::Controller::WheelMoved(QWheelEvent* event)
{
    if (!mSuccess)
        return;

    for (const auto& manager : qAsConst(mManagers))
        manager->WheelMoved(event);
}

void Canavar::Engine::Controller::MousePressed(QMouseEvent* event)
{
    if (!mSuccess)
        return;

    for (const auto& manager : qAsConst(mManagers))
        manager->MousePressed(event);
}

void Canavar::Engine::Controller::MouseReleased(QMouseEvent* event)
{
    if (!mSuccess)
        return;

    for (const auto& manager : qAsConst(mManagers))
        manager->MouseReleased(event);
}

void Canavar::Engine::Controller::MouseMoved(QMouseEvent* event)
{
    if (!mSuccess)
        return;

    for (const auto& manager : qAsConst(mManagers))
        manager->MouseMoved(event);
}

void Canavar::Engine::Controller::KeyPressed(QKeyEvent* event)
{
    if (!mSuccess)
        return;

    for (const auto& manager : qAsConst(mManagers))
        manager->KeyPressed(event);
}

void Canavar::Engine::Controller::KeyReleased(QKeyEvent* event)
{
    if (!mSuccess)
        return;

    for (const auto& manager : qAsConst(mManagers))
        manager->KeyReleased(event);
}

void Canavar::Engine::Controller::Resize(int w, int h)
{
    if (!mSuccess)
        return;

    mWindow->makeCurrent();

    glViewport(0, 0, w * mWindow->devicePixelRatioF(), h * mWindow->devicePixelRatioF());

    for (const auto& manager : qAsConst(mManagers))
        manager->Resize(w * mWindow->devicePixelRatioF(), h * mWindow->devicePixelRatioF());

    mWindow->doneCurrent();
}