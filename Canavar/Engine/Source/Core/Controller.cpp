#include "Controller.h"

#include "Constants.h"
#include "Core/Window.h"
#include "Manager/CameraManager.h"
#include "Manager/LightManager.h"
#include "Manager/NodeManager.h"
#include "Manager/RenderingManager.h"
#include "Manager/ShaderManager.h"
#include "Node/Model/Model.h"
#include "Util/Logger.h"

#include <imgui.h>

#include <QThread>
#include <QtImGui.h>

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

    mManagers.push_back(mShaderManager);
    mManagers.push_back(mNodeManager);
    mManagers.push_back(mCameraManager);
    mManagers.push_back(mRenderingManager);
    mManagers.push_back(mLightManager);
}

Canavar::Engine::Controller::~Controller()
{
    qDebug() << "Controller::~Controller: Application closing...";
    qDebug() << "Controller::~Controller: Current Thread:" << QThread::currentThread();
}

void Canavar::Engine::Controller::Run()
{
    qDebug() << "Controller::Controller: Application starting...";

    Q_INIT_RESOURCE(Engine);

    mWindow->show();
    mWindow->resize(INITIAL_WIDTH, INITIAL_HEIGHT);
}

void Canavar::Engine::Controller::Initialize()
{
    qInstallMessageHandler(Logger::QtMessageOutputCallback);

    for (const auto pManager : mManagers)
    {
        pManager->SetManagerProvider(this);
        pManager->Initialize();
    }

    for (const auto pManager : mManagers)
    {
        pManager->PostInitialize();
    }

    QtImGui::initialize(mWindow);

    // Test
    ModelPtr pModel = std::make_shared<Model>("f16c");
    mNodeManager->AddNode(pModel);
}

void Canavar::Engine::Controller::Render(float ifps)
{
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

    QtImGui::newFrame();

    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // glViewport(0, 0, mWindow->width(), mWindow->height());

    // ImGui::ShowDemoWindow();

    ImGui::Render();
    QtImGui::render();
}

void Canavar::Engine::Controller::OnKeyPressed(QKeyEvent* event)
{
    mCameraManager->OnKeyPressed(event);
}

void Canavar::Engine::Controller::OnKeyReleased(QKeyEvent* event)
{
    mCameraManager->OnKeyReleased(event);
}

void Canavar::Engine::Controller::Resize(int width, int height)
{
    mWindow->makeCurrent();
    mRenderingManager->Resize(width, height);
    mCameraManager->Resize(width, height);
    mWindow->doneCurrent();
}

void Canavar::Engine::Controller::OnMousePressed(QMouseEvent* event)
{
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    mCameraManager->OnMousePressed(event);
}

void Canavar::Engine::Controller::OnMouseReleased(QMouseEvent* event)
{
    mCameraManager->OnMouseReleased(event);
}

void Canavar::Engine::Controller::OnMouseMoved(QMouseEvent* event)
{
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    mCameraManager->OnMouseMoved(event);
}

void Canavar::Engine::Controller::OnWheelMoved(QWheelEvent* event)
{
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    mCameraManager->OnWheelMoved(event);
}
