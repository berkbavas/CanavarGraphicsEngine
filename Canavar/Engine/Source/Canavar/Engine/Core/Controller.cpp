#include "Controller.h"

#include "Canavar/Engine/Core/Constants.h"
#include "Canavar/Engine/Core/EventReceiver.h"
#include "Canavar/Engine/Core/Widget.h"
#include "Canavar/Engine/Core/Window.h"
#include "Canavar/Engine/Manager/CameraManager.h"
#include "Canavar/Engine/Manager/LightManager.h"
#include "Canavar/Engine/Manager/NodeManager.h"
#include "Canavar/Engine/Manager/RenderingManager/RenderingManager.h"
#include "Canavar/Engine/Manager/ShaderManager.h"
#include "Canavar/Engine/Util/Logger.h"

#include <QThread>

Canavar::Engine::Controller::Controller(RenderingContext* pRenderingContext, QObject* pParent)
    : QObject(pParent)
    , mRenderingContext(pRenderingContext)
{
    CGE_ASSERT(mRenderingContext != nullptr);

    if (Window* pWindow = dynamic_cast<Window*>(mRenderingContext))
    {
        connect(pWindow, &Window::Initialize, this, &Controller::Initialize);
        connect(pWindow, &Window::Render, this, &Controller::Render);
        connect(pWindow, &Window::Resize, this, &Controller::Resize);
        connect(pWindow, &Window::MousePressed, this, &Controller::OnMousePressed);
        connect(pWindow, &Window::MouseReleased, this, &Controller::OnMouseReleased);
        connect(pWindow, &Window::MouseMoved, this, &Controller::OnMouseMoved);
        connect(pWindow, &Window::WheelMoved, this, &Controller::OnWheelMoved);
        connect(pWindow, &Window::KeyPressed, this, &Controller::OnKeyPressed);
        connect(pWindow, &Window::KeyReleased, this, &Controller::OnKeyReleased);
    }
    else if (Widget* pWidget = dynamic_cast<Widget*>(mRenderingContext))
    {
        connect(pWidget, &Widget::Initialize, this, &Controller::Initialize);
        connect(pWidget, &Widget::Render, this, &Controller::Render);
        connect(pWidget, &Widget::Resize, this, &Controller::Resize);
        connect(pWidget, &Widget::MousePressed, this, &Controller::OnMousePressed);
        connect(pWidget, &Widget::MouseReleased, this, &Controller::OnMouseReleased);
        connect(pWidget, &Widget::MouseMoved, this, &Controller::OnMouseMoved);
        connect(pWidget, &Widget::WheelMoved, this, &Controller::OnWheelMoved);
        connect(pWidget, &Widget::KeyPressed, this, &Controller::OnKeyPressed);
        connect(pWidget, &Widget::KeyReleased, this, &Controller::OnKeyReleased);
    }
    else
    {
        CGE_EXIT_FAILURE("Controller: Unsupported rendering context!");
    }

    mShaderManager = new ShaderManager(nullptr);
    mNodeManager = new NodeManager(nullptr);
    mCameraManager = new CameraManager(nullptr);
    mRenderingManager = new RenderingManager(nullptr);
    mLightManager = new LightManager(nullptr);

    mManagers.push_back(mShaderManager);
    mManagers.push_back(mNodeManager);
    mManagers.push_back(mCameraManager);
    mManagers.push_back(mRenderingManager);
    mManagers.push_back(mLightManager);

    connect(mRenderingManager, &RenderingManager::RenderLoop, this, &Controller::OnRenderLoop, Qt::DirectConnection);
}

Canavar::Engine::Controller::~Controller()
{
    qDebug() << "Controller::~Controller: Current Thread:" << QThread::currentThread();
    qDebug() << "Controller::~Controller: Application closing...";

    mRenderingContext->MakeCurrent();

    for (const auto pManager : mManagers)
    {
        pManager->Shutdown();
    }

    for (const auto pReceiver : mEventReceivers)
    {
        pReceiver->Shutdown();
    }

    mRenderingContext->DoneCurrent();
}

Canavar::Engine::NodeManager* Canavar::Engine::Controller::GetNodeManager()
{
    return mNodeManager;
}

Canavar::Engine::ShaderManager* Canavar::Engine::Controller::GetShaderManager()
{
    return mShaderManager;
}

Canavar::Engine::CameraManager* Canavar::Engine::Controller::GetCameraManager()
{
    return mCameraManager;
}

Canavar::Engine::LightManager* Canavar::Engine::Controller::GetLightManager()
{
    return mLightManager;
}

Canavar::Engine::RenderingManager* Canavar::Engine::Controller::GetRenderingManager()
{
    return mRenderingManager;
}

void Canavar::Engine::Controller::Initialize()
{
    qInstallMessageHandler(Logger::QtMessageOutputCallback);

    qDebug() << "Controller::Controller: Application starting...";

    Q_INIT_RESOURCE(Engine);
    Q_INIT_RESOURCE(EngineBigObjects);

    for (const auto pManager : mManagers)
    {
        pManager->SetManagerProvider(this);
        pManager->Initialize();
    }

    for (const auto pManager : mManagers)
    {
        pManager->PostInitialize();
    }

    for (const auto pReceiver : mEventReceivers)
    {
        pReceiver->Initialize();
    }

    AddEventReceiver(mCameraManager);
}

void Canavar::Engine::Controller::Render(float ifps)
{
    mDevicePixelRatio = mRenderingContext->GetDevicePixelRatio();

    for (const auto pManager : mManagers)
    {
        pManager->SetDevicePixelRatio(mDevicePixelRatio);
    }

    for (const auto pReceiver : mEventReceivers)
    {
        pReceiver->Update(ifps);
    }

    for (const auto pManager : mManagers)
    {
        pManager->Update(ifps);
    }

    for (const auto pManager : mManagers)
    {
        pManager->Render(ifps);
    }

    for (const auto pReceiver : mEventReceivers)
    {
        pReceiver->PostRender(ifps);
    }
}

void Canavar::Engine::Controller::OnRenderLoop(float ifps)
{
    for (const auto pReceiver : mEventReceivers)
    {
        pReceiver->Render(ifps);
    }
}

void Canavar::Engine::Controller::OnKeyPressed(QKeyEvent* event)
{
    for (const auto pReceiver : mEventReceivers)
    {
        if (pReceiver->KeyPressed(event))
        {
            return;
        }
    }
}

void Canavar::Engine::Controller::OnKeyReleased(QKeyEvent* event)
{
    for (const auto pReceiver : mEventReceivers)
    {
        if (pReceiver->KeyReleased(event))
        {
            return;
        }
    }
}

void Canavar::Engine::Controller::AddEventReceiver(EventReceiver* pReceiver)
{
    mEventReceivers.push_back(pReceiver);
}

void Canavar::Engine::Controller::RemoveEventReceiver(EventReceiver* pReceiver)
{
    mEventReceivers.removeAll(pReceiver);
}

void Canavar::Engine::Controller::Resize(int width, int height)
{
    mDevicePixelRatio = mRenderingContext->GetDevicePixelRatio();

    mWidth = width * mDevicePixelRatio;
    mHeight = height * mDevicePixelRatio;

    mRenderingContext->MakeCurrent();

    mRenderingManager->Resize(mWidth, mHeight);
    mCameraManager->Resize(mWidth, mHeight);

    for (const auto pReceiver : mEventReceivers)
    {
        pReceiver->Resize(mWidth, mHeight);
    }
    mRenderingContext->DoneCurrent();
}

void Canavar::Engine::Controller::OnMousePressed(QMouseEvent* event)
{
    for (const auto pReceiver : mEventReceivers)
    {
        if (pReceiver->MousePressed(event))
        {
            return;
        }
    }
}

void Canavar::Engine::Controller::OnMouseReleased(QMouseEvent* event)
{
    for (const auto pReceiver : mEventReceivers)
    {
        if (pReceiver->MouseReleased(event))
        {
            return;
        }
    }
}

void Canavar::Engine::Controller::OnMouseMoved(QMouseEvent* event)
{
    for (const auto pReceiver : mEventReceivers)
    {
        if (pReceiver->MouseMoved(event))
        {
            return;
        }
    }
}

void Canavar::Engine::Controller::OnWheelMoved(QWheelEvent* event)
{
    for (const auto pReceiver : mEventReceivers)
    {
        if (pReceiver->WheelMoved(event))
        {
            return;
        }
    }
}
