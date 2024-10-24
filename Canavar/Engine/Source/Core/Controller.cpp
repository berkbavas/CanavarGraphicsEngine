#include "Controller.h"

#include "Core/Window.h"

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
}

Canavar::Engine::Controller::~Controller()
{
    qDebug() << "Controller::~Controller: Application closing...";
    qDebug() << "Controller::~Controller: Current Thread:" << QThread::currentThread();
}

void Canavar::Engine::Controller::Run()
{
    qDebug() << "Controller::Controller: Application starting...";

    mWindow->resize(1600, 900);
    mWindow->show();
}

void Canavar::Engine::Controller::Initialize()
{
    initializeOpenGLFunctions();
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);


    QtImGui::initialize(mWindow);
}

void Canavar::Engine::Controller::Render(float ifps)
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    QtImGui::newFrame();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, mWindow->width(), mWindow->height());

    ImGui::ShowDemoWindow();

    ImGui::Render();
    QtImGui::render();
}

void Canavar::Engine::Controller::OnKeyPressed(QKeyEvent* event)
{
}

void Canavar::Engine::Controller::OnKeyReleased(QKeyEvent* event)
{
}

void Canavar::Engine::Controller::Resize(int width, int height)
{
}

void Canavar::Engine::Controller::OnMousePressed(QMouseEvent* event)
{
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }
}

void Canavar::Engine::Controller::OnMouseReleased(QMouseEvent* event)
{
}

void Canavar::Engine::Controller::OnMouseMoved(QMouseEvent* event)
{
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }
}

void Canavar::Engine::Controller::OnWheelMoved(QWheelEvent* event)
{
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }
}
