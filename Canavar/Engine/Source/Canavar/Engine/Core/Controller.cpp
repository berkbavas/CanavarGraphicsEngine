#include "Controller.h"

#include "Canavar/Engine/Core/Constants.h"
#include "Canavar/Engine/Core/EventReceiver.h"
#include "Canavar/Engine/Core/Window.h"
#include "Canavar/Engine/Manager/CameraManager.h"
#include "Canavar/Engine/Manager/LightManager.h"
#include "Canavar/Engine/Manager/NodeManager.h"
#include "Canavar/Engine/Manager/RenderingManager/RenderingManager.h"
#include "Canavar/Engine/Manager/ShaderManager.h"
#include "Canavar/Engine/Manager/VertexPainter.h"
#include "Canavar/Engine/Util/Logger.h"

#include <QThread>

Canavar::Engine::Controller::Controller(QObject* parent)
    : QObject(parent)
{
    mWindow = new Window;

    connect(mWindow, &Window::Initialize, this, &Controller::Initialize);
    connect(mWindow, &Window::Render, this, &Controller::Render);
    connect(mWindow, &Window::Resize, this, &Controller::Resize);
    connect(mWindow, &Window::MousePressed, this, &Controller::OnMousePressed);
    connect(mWindow, &Window::MouseReleased, this, &Controller::OnMouseReleased);
    connect(mWindow, &Window::MouseMoved, this, &Controller::OnMouseMoved);
    connect(mWindow, &Window::WheelMoved, this, &Controller::OnWheelMoved);
    connect(mWindow, &Window::KeyPressed, this, &Controller::OnKeyPressed);
    connect(mWindow, &Window::KeyReleased, this, &Controller::OnKeyReleased);

    mNodeManager = new NodeManager(nullptr);
    mCameraManager = new CameraManager(nullptr);
    mShaderManager = new ShaderManager(nullptr);
    mRenderingManager = new RenderingManager(nullptr);
    mLightManager = new LightManager(nullptr);
    mVertexPainter = new VertexPainter(nullptr);

    mManagers.push_back(mShaderManager);
    mManagers.push_back(mVertexPainter);
    mManagers.push_back(mNodeManager);
    mManagers.push_back(mCameraManager);
    mManagers.push_back(mRenderingManager);
    mManagers.push_back(mLightManager);

    connect(mRenderingManager, &RenderingManager::RenderLoop, this, &Controller::onRenderLoop, Qt::DirectConnection);
}

Canavar::Engine::Controller::~Controller()
{
    qDebug() << "Controller::~Controller: Current Thread:" << QThread::currentThread();
    qDebug() << "Controller::~Controller: Application closing...";
}

void Canavar::Engine::Controller::Run()
{
    qDebug() << "Controller::Controller: Application starting...";

    qInstallMessageHandler(Logger::QtMessageOutputCallback);

    Q_INIT_RESOURCE(Engine);

    mWindow->setWidth(INITIAL_WIDTH);
    mWindow->setHeight(INITIAL_HEIGHT);
    mWindow->showMinimized();
}

void Canavar::Engine::Controller::Initialize()
{
    initializeOpenGLFunctions();
    mWindow->showMaximized();

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
}

void Canavar::Engine::Controller::Render(float ifps)
{
    mDevicePixelRatio = mWindow->devicePixelRatio();

    for (const auto pManager : mManagers)
    {
        pManager->SetDevicePixelRatio(mDevicePixelRatio);
    }

    for (const auto pManager : mManagers)
    {
        pManager->PreUpdate(ifps);
    }

    for (const auto pManager : mManagers)
    {
        pManager->Update(ifps);
    }

    for (const auto pManager : mManagers)
    {
        pManager->PostUpdate(ifps);
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

void Canavar::Engine::Controller::onRenderLoop(float ifps)
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

    mCameraManager->OnKeyPressed(event);
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

    mCameraManager->OnKeyReleased(event);
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
    mDevicePixelRatio = mWindow->devicePixelRatio();
    mWidth = width * mDevicePixelRatio;
    mHeight = height * mDevicePixelRatio;

    mWindow->makeCurrent();

    mRenderingManager->Resize(mWidth, mHeight);
    mCameraManager->Resize(mWidth, mHeight);

    for (const auto pReceiver : mEventReceivers)
    {
        pReceiver->Resize(mWidth, mHeight);
    }

    mWindow->doneCurrent();
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

    mCameraManager->OnMousePressed(event);
    mVertexPainter->OnMousePressed(event);
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

    mCameraManager->OnMouseReleased(event);
    mVertexPainter->OnMouseReleased(event);
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

    mCameraManager->OnMouseMoved(event);
    mVertexPainter->OnMouseMoved(event);
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

    mCameraManager->OnWheelMoved(event);
    mVertexPainter->OnWheelMoved(event);
}
