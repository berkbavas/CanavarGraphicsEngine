#include "Controller.h"

#include "Canavar/Engine/Core/Constants.h"
#include "Canavar/Engine/Core/EventReceiver.h"
#include "Canavar/Engine/Core/Widget.h"
#include "Canavar/Engine/Core/Window.h"
#include "Canavar/Engine/Manager/BoundingBoxRenderer.h"
#include "Canavar/Engine/Manager/CameraManager.h"
#include "Canavar/Engine/Manager/LightManager.h"
#include "Canavar/Engine/Manager/NodeManager.h"
#include "Canavar/Engine/Manager/Painter.h"
#include "Canavar/Engine/Manager/RenderingManager.h"
#include "Canavar/Engine/Manager/ShaderManager.h"
#include "Canavar/Engine/Manager/ShadowMappingRenderer.h"
#include "Canavar/Engine/Manager/TextRenderer.h"
#include "Canavar/Engine/Util/Logger.h"

#include <imgui.h>

#include <QThread>
#include <QtImGui.h>

Canavar::Engine::Controller::Controller(RenderingContext* pRenderingContext, bool WithImGui, QObject* pParent)
    : QObject(pParent)
    , mRenderingContext(pRenderingContext)
{
    CGE_ASSERT(mRenderingContext != nullptr);
    mWindow = dynamic_cast<Window*>(mRenderingContext);
    mWidget = dynamic_cast<Widget*>(mRenderingContext);

    if (WithImGui)
    {
        mImGuiWidget = new ImGuiWidget(mRenderingContext, this);
    }

    if (mWindow)
    {
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
        CGE_EXIT_FAILURE("Controller::Controller: Unsupported rendering context!");
    }

    mShaderManager = new ShaderManager(mRenderingContext, this);
    mNodeManager = new NodeManager(mRenderingContext, this);
    mCameraManager = new CameraManager(mRenderingContext, this);
    mRenderingManager = new RenderingManager(mRenderingContext, this);
    mLightManager = new LightManager(mRenderingContext, this);
    mPainter = new Painter(mRenderingContext, this);
    mTextRenderer = new TextRenderer(mRenderingContext, this);
    mShadowMappingRenderer = new ShadowMappingRenderer(mRenderingContext, this);
    mBoundingBoxRenderer = new BoundingBoxRenderer(mRenderingContext, this);

    mRenderingContext->mShaderManager = mShaderManager;
    mRenderingContext->mNodeManager = mNodeManager;
    mRenderingContext->mCameraManager = mCameraManager;
    mRenderingContext->mRenderingManager = mRenderingManager;
    mRenderingContext->mLightManager = mLightManager;
    mRenderingContext->mPainter = mPainter;
    mRenderingContext->mTextRenderer = mTextRenderer;
    mRenderingContext->mShadowMappingRenderer = mShadowMappingRenderer;
    mRenderingContext->mBoundingBoxRenderer = mBoundingBoxRenderer;

    mManagers.push_back(mPainter);
    mManagers.push_back(mShaderManager);
    mManagers.push_back(mNodeManager);
    mManagers.push_back(mCameraManager);
    mManagers.push_back(mRenderingManager);
    mManagers.push_back(mLightManager);
    mManagers.push_back(mTextRenderer);
    mManagers.push_back(mShadowMappingRenderer);
    mManagers.push_back(mBoundingBoxRenderer);

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
}

void Canavar::Engine::Controller::Initialize()
{
    qInstallMessageHandler(Logger::QtMessageOutputCallback);

    qDebug() << "Controller::Controller: Application starting...";

    Q_INIT_RESOURCE(Engine);
    Q_INIT_RESOURCE(EngineBigObjects);

    if (mImGuiWidget)
    {
        AddEventReceiver(mImGuiWidget);
    }

    AddEventReceiver(mPainter);
    AddEventReceiver(mShaderManager);
    AddEventReceiver(mNodeManager);
    AddEventReceiver(mCameraManager);
    AddEventReceiver(mRenderingManager);
    AddEventReceiver(mLightManager);
    AddEventReceiver(mTextRenderer);
    AddEventReceiver(mShadowMappingRenderer);
    AddEventReceiver(mBoundingBoxRenderer);

    for (const auto pManager : mManagers)
    {
        pManager->Initialize();
    }

    for (const auto pManager : mManagers)
    {
        pManager->PostInitialize();
    }

    if (mImGuiWidget)
    {
        mImGuiWidget->PostInitialize();

        if (mWindow)
        {
            mRenderRef = QtImGui::initialize(mWindow, false);
        }
        else if (mWidget)
        {
            mRenderRef = QtImGui::initialize(mWidget, false);
        }
        else
        {
            CGE_EXIT_FAILURE("Controller::Initialize: Unsupported rendering context for ImGui!");
        }

        connect(mImGuiWidget,
                &ImGuiWidget::GoToObject,
                this,
                [=](Engine::Object* pObject) //
                {
                    mCameraManager->SetActiveCamera(mCameraManager->GetFreeCamera());
                    mCameraManager->GetFreeCamera()->GoToObject(pObject);
                });
    }

    emit Initialized();
    emit PostInitialized();
}

void Canavar::Engine::Controller::Render(float ifps)
{
    mDevicePixelRatio = mRenderingContext->GetDevicePixelRatio();

    mCameraManager->SetDevicePixelRatio(mDevicePixelRatio);

    auto* pActiveCamera = mCameraManager->GetActiveCamera();

    if (pActiveCamera == nullptr)
    {
        return;
    }

    for (const auto pManager : mManagers)
    {
        pManager->Update(ifps);
    }

    emit Updated(ifps);

    for (const auto pManager : mManagers)
    {
        pManager->Render(pActiveCamera);
    }

    for (const auto pManager : mManagers)
    {
        pManager->PostRender(ifps);
    }

    emit PostRendered(ifps);

    if (mImGuiWidget)
    {
        QtImGui::newFrame(mRenderRef);
        mImGuiWidget->DrawImGui(ifps);
        emit DrawImGui(ifps);
        ImGui::Render();
        QtImGui::render(mRenderRef);
    }
}

void Canavar::Engine::Controller::OnRenderLoop()
{
    auto* pActiveCamera = mCameraManager->GetActiveCamera();

    for (const auto pManager : mManagers)
    {
        pManager->InRender(pActiveCamera);
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
    mWidth = width;
    mHeight = height;

    mRenderingContext->MakeCurrent();

    for (const auto pManager : mManagers)
    {
        pManager->Resize(mWidth, mHeight);
    }
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
}
