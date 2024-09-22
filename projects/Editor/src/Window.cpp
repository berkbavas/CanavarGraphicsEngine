#include "Window.h"

#include <Controller.h>
#include <Gui.h>
#include <PerspectiveCamera.h>

#include <QDateTime>
#include <QKeyEvent>
#include <QVector3D>

#include <QDebug>

using namespace Canavar::Engine;

Window::Window(QWindow *parent)
    : QOpenGLWindow(QOpenGLWindow::UpdateBehavior::NoPartialUpdate, parent)

{
    //QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    //format.setMajorVersion(4);
    //format.setMinorVersion(3);
    //format.setProfile(QSurfaceFormat::CoreProfile);
    //format.setSamples(0);
    //format.setSwapInterval(1);
    //setFormat(format);

    connect(this, &QOpenGLWindow::frameSwapped, this, [=]() { update(); });
}

void Window::initializeGL()
{
    initializeOpenGLFunctions();

    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    mPreviousTime = mCurrentTime;

    QtImGui::initialize(this);

    mController = new Canavar::Engine::Controller;
    mController->SetWindow(this);

    if (mController->Init("resources/Config/Config.json"))
    {
        mCamera = dynamic_cast<PerspectiveCamera *>(NodeManager::Instance()->GetNodeByName("Free Camera"));
        CameraManager::Instance()->SetActiveCamera(mCamera);

        mGui = new Gui;
    }
}

void Window::resizeGL(int w, int h)
{
    mController->Resize(w, h);
    mGui->Resize(w, h);
}

void Window::paintGL()
{
    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    float ifps = (mCurrentTime - mPreviousTime) * 0.001f;
    mPreviousTime = mCurrentTime;

    mController->Render(ifps);

    QtImGui::newFrame();
    mGui->Draw();
    glViewport(0, 0, width() * devicePixelRatioF(), height() * devicePixelRatioF());
    ImGui::Render();
    QtImGui::render();
}

void Window::keyPressEvent(QKeyEvent *event)
{
    if (ImGui::GetIO().WantCaptureKeyboard)
        return;

    mController->KeyPressed(event);
    mGui->KeyPressed(event);
}

void Window::keyReleaseEvent(QKeyEvent *event)
{
    mController->KeyReleased(event);
}

void Window::mousePressEvent(QMouseEvent *event)
{
    if (ImGui::GetIO().WantCaptureMouse)
        return;

    mController->MousePressed(event);
    mGui->MousePressed(event);
}

void Window::mouseReleaseEvent(QMouseEvent *event)
{
    mController->MouseReleased(event);
    mGui->MouseReleased(event);
}

void Window::mouseMoveEvent(QMouseEvent *event)
{
    if (ImGui::GetIO().WantCaptureMouse)
        return;

    mController->MouseMoved(event);
    mGui->MouseMoved(event);
}

void Window::wheelEvent(QWheelEvent *event)
{
    if (ImGui::GetIO().WantCaptureMouse)
        return;

    mController->WheelMoved(event);
}
