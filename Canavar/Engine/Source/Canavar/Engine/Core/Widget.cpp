#include "Widget.h"

#include <QDateTime>
#include <QDebug>
#include <QKeyEvent>

Canavar::Engine::Widget::Widget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    connect(this, &QOpenGLWidget::frameSwapped, [this]() { update(); });

    setMouseTracking(true);
}

void Canavar::Engine::Widget::initializeGL()
{
    initializeOpenGLFunctions();

    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    mPreviousTime = mCurrentTime;

    emit Initialize();
}

void Canavar::Engine::Widget::resizeGL(int width, int height)
{
    emit Resize(width, height);
}

void Canavar::Engine::Widget::paintGL()
{
    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    const float ifps = (mCurrentTime - mPreviousTime) * 0.001f;
    mPreviousTime = mCurrentTime;

    emit Render(ifps);
}

void Canavar::Engine::Widget::keyPressEvent(QKeyEvent *event)
{
    emit KeyPressed(event);
}

void Canavar::Engine::Widget::keyReleaseEvent(QKeyEvent *event)
{
    emit KeyReleased(event);
}

void Canavar::Engine::Widget::mousePressEvent(QMouseEvent *event)
{
    emit MousePressed(event);
}

void Canavar::Engine::Widget::mouseReleaseEvent(QMouseEvent *event)
{
    emit MouseReleased(event);
}

void Canavar::Engine::Widget::mouseMoveEvent(QMouseEvent *event)
{
    emit MouseMoved(event);
}

void Canavar::Engine::Widget::wheelEvent(QWheelEvent *event)
{
    emit WheelMoved(event);
}

bool Canavar::Engine::Widget::eventFilter(QObject *pReceiver, QEvent *pEvent)
{
    if (pEvent->type() == QEvent::MouseButtonPress)
    {
        mousePressEvent(dynamic_cast<QMouseEvent *>(pEvent));
        return true;
    }
    else if (pEvent->type() == QEvent::MouseButtonRelease)
    {
        mouseReleaseEvent(dynamic_cast<QMouseEvent *>(pEvent));
        return true;
    }
    else if (pEvent->type() == QEvent::MouseMove)
    {
        mouseMoveEvent(dynamic_cast<QMouseEvent *>(pEvent));
        return true;
    }
    else if (pEvent->type() == QEvent::Wheel)
    {
        wheelEvent(dynamic_cast<QWheelEvent *>(pEvent));
        return true;
    }
    else if (pEvent->type() == QEvent::KeyPress)
    {
        keyPressEvent(dynamic_cast<QKeyEvent *>(pEvent));
        return true;
    }
    else if (pEvent->type() == QEvent::KeyRelease)
    {
        keyReleaseEvent(dynamic_cast<QKeyEvent *>(pEvent));
        return true;
    }

    return false;
}
