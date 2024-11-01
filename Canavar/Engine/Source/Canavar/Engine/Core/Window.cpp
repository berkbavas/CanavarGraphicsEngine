#include "Window.h"

#include <QDateTime>
#include <QDebug>
#include <QKeyEvent>

Canavar::Engine::Window::Window(QWindow *parent)
    : QOpenGLWindow(QOpenGLWindow::UpdateBehavior::NoPartialUpdate, parent)
{
    connect(this, &QOpenGLWindow::frameSwapped, [this]() { update(); });
}

void Canavar::Engine::Window::initializeGL()
{
    initializeOpenGLFunctions();

    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    mPreviousTime = mCurrentTime;

    emit Initialize();
}

void Canavar::Engine::Window::resizeGL(int width, int height)
{
    emit Resize(width, height);
}

void Canavar::Engine::Window::paintGL()
{
    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    const float ifps = (mCurrentTime - mPreviousTime) * 0.001f;
    mPreviousTime = mCurrentTime;

    emit Render(ifps);
}

void Canavar::Engine::Window::keyPressEvent(QKeyEvent *event)
{
    emit KeyPressed(event);
}

void Canavar::Engine::Window::keyReleaseEvent(QKeyEvent *event)
{
    emit KeyReleased(event);
}

void Canavar::Engine::Window::mousePressEvent(QMouseEvent *event)
{
    emit MousePressed(event);
}

void Canavar::Engine::Window::mouseReleaseEvent(QMouseEvent *event)
{
    emit MouseReleased(event);
}

void Canavar::Engine::Window::mouseMoveEvent(QMouseEvent *event)
{
    emit MouseMoved(event);
}

void Canavar::Engine::Window::wheelEvent(QWheelEvent *event)
{
    emit WheelMoved(event);
}
