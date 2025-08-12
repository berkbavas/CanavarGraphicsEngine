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

#include <imgui.h>

#include <QThread>
#include <QtImGui.h>

Canavar::Engine::Controller::Controller(RenderingContext* pRenderingContext, bool withImGui, QObject* pParent)
    : QObject(pParent)
    , mRenderingContext(pRenderingContext)
{
    CGE_ASSERT(mRenderingContext != nullptr);
    mWindow = dynamic_cast<Window*>(mRenderingContext);
    mWidget = dynamic_cast<Widget*>(mRenderingContext);

    if (mWindow)
    {
        if (withImGui)
        {
            mImGuiWidget = new ImGuiWidget(this);
        }

        connect(mWindow, &Window::Initialize, this, &Controller::Initialize);
        connect(mWindow, &Window::Render, this, &Controller::Render);
        connect(mWindow, &Window::Resize, this, &Controller::Resize);
        connect(mWindow, &Window::MousePressed, this, &Controller::OnMousePressed);
        connect(mWindow, &Window::MouseReleased, this, &Controller::OnMouseReleased);
        connect(mWindow, &Window::MouseMoved, this, &Controller::OnMouseMoved);
        connect(mWindow, &Window::WheelMoved, this, &Controller::OnWheelMoved);
        connect(mWindow, &Window::KeyPressed, this, &Controller::OnKeyPressed);
        connect(mWindow, &Window::KeyReleased, this, &Controller::OnKeyReleased);
    }
    else if (mWidget)
    {
        if (withImGui)
        {
            mImGuiWidget = new ImGuiWidget(this);
        }

        connect(mWidget, &Widget::Initialize, this, &Controller::Initialize);
        connect(mWidget, &Widget::Render, this, &Controller::Render);
        connect(mWidget, &Widget::Resize, this, &Controller::Resize);
        connect(mWidget, &Widget::MousePressed, this, &Controller::OnMousePressed);
        connect(mWidget, &Widget::MouseReleased, this, &Controller::OnMouseReleased);
        connect(mWidget, &Widget::MouseMoved, this, &Controller::OnMouseMoved);
        connect(mWidget, &Widget::WheelMoved, this, &Controller::OnWheelMoved);
        connect(mWidget, &Widget::KeyPressed, this, &Controller::OnKeyPressed);
        connect(mWidget, &Widget::KeyReleased, this, &Controller::OnKeyReleased);
    }
    else
    {
        CGE_EXIT_FAILURE("Controller: Unsupported rendering context!");
    }

    mShaderManager = new ShaderManager(this);
    mNodeManager = new NodeManager(this);
    mCameraManager = new CameraManager(this);
    mRenderingManager = new RenderingManager(this);
    mLightManager = new LightManager(this);

    if (mImGuiWidget)
    {
        mImGuiWidget->SetCameraManager(mCameraManager);
        mImGuiWidget->SetNodeManager(mNodeManager);
        mImGuiWidget->SetRenderingManager(mRenderingManager);
        AddEventReceiver(mImGuiWidget);
    }

    AddEventReceiver(mShaderManager);
    AddEventReceiver(mNodeManager);
    AddEventReceiver(mCameraManager);
    AddEventReceiver(mRenderingManager);
    AddEventReceiver(mLightManager);

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
    }

    for (const auto pReceiver : mEventReceivers)
    {
        pReceiver->Initialize();
    }

    for (const auto pReceiver : mEventReceivers)
    {
        pReceiver->PostInitialize();
    }

    if (mImGuiWidget)
    {
        if (mWindow)
        {
            mRenderRef = QtImGui::initialize(mWindow);
        }
        else if (mWidget)
        {
            mRenderRef = QtImGui::initialize(mWidget);
        }
        else
        {
            CGE_EXIT_FAILURE("Controller::Initialize: Unsupported rendering context for ImGui!");
        }

        connect(mImGuiWidget, &ImGuiWidget::GoToObject, this, [=](Engine::ObjectPtr pObject) {
            mCameraManager->SetActiveCamera(mCameraManager->GetFreeCamera());
            mCameraManager->GetFreeCamera()->GoToObject(pObject);
        });
    }
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

    for (const auto pReceiver : mEventReceivers)
    {
        pReceiver->Render(ifps);
    }

    for (const auto pReceiver : mEventReceivers)
    {
        pReceiver->PostRender(ifps);
    }

    QtImGui::newFrame(mRenderRef);

    for (const auto pReceiver : mEventReceivers)
    {
        pReceiver->DrawImGui(ifps);
    }

    ImGui::Render();
    QtImGui::render(mRenderRef);
}

void Canavar::Engine::Controller::OnRenderLoop(float ifps)
{
    for (const auto pReceiver : mEventReceivers)
    {
        pReceiver->InRender(ifps);
    }
}

void Canavar::Engine::Controller::OnKeyPressed(QKeyEvent* event)
{
    mRenderingContext->MakeCurrent();

    for (const auto pReceiver : mEventReceivers)
    {
        if (pReceiver->KeyPressed(event))
        {
            return;
        }
    }

    mRenderingContext->DoneCurrent();
}

void Canavar::Engine::Controller::OnKeyReleased(QKeyEvent* event)
{
    mRenderingContext->MakeCurrent();

    for (const auto pReceiver : mEventReceivers)
    {
        if (pReceiver->KeyReleased(event))
        {
            return;
        }
    }

    mRenderingContext->DoneCurrent();
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

    for (const auto pReceiver : mEventReceivers)
    {
        pReceiver->Resize(mWidth, mHeight);
    }

    mRenderingContext->DoneCurrent();
}

void Canavar::Engine::Controller::OnMousePressed(QMouseEvent* event)
{
    mRenderingContext->MakeCurrent();

    for (const auto pReceiver : mEventReceivers)
    {
        if (pReceiver->MousePressed(event))
        {
            return;
        }
    }

    mRenderingContext->DoneCurrent();
}

void Canavar::Engine::Controller::OnMouseReleased(QMouseEvent* event)
{
    mRenderingContext->MakeCurrent();

    for (const auto pReceiver : mEventReceivers)
    {
        if (pReceiver->MouseReleased(event))
        {
            return;
        }
    }

    mRenderingContext->DoneCurrent();
}

void Canavar::Engine::Controller::OnMouseMoved(QMouseEvent* event)
{
    mRenderingContext->MakeCurrent();

    for (const auto pReceiver : mEventReceivers)
    {
        if (pReceiver->MouseMoved(event))
        {
            return;
        }
    }

    mRenderingContext->DoneCurrent();
}

void Canavar::Engine::Controller::OnWheelMoved(QWheelEvent* event)
{
    mRenderingContext->MakeCurrent();

    for (const auto pReceiver : mEventReceivers)
    {
        if (pReceiver->WheelMoved(event))
        {
            return;
        }
    }

    mRenderingContext->DoneCurrent();
}
