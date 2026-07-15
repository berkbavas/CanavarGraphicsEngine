#include "OpenGLWidget.h"

#include "Canavar/Engine/Util/Logger.h"
#include "Canavar/Engine/Util/Util.h"

#include <QDateTime>

Canavar::Engine::OpenGLWidget::OpenGLWidget(QWidget *pParent)
    : QOpenGLWidget(pParent)
{
    connect(this, &OpenGLWidget::frameSwapped, this, [this]() { update(); });
    setMouseTracking(true);
}

void Canavar::Engine::OpenGLWidget::MakeCurrent()
{
    makeCurrent();
}

void Canavar::Engine::OpenGLWidget::DoneCurrent()
{
    doneCurrent();
}

float Canavar::Engine::OpenGLWidget::GetDevicePixelRatio() const
{
    return devicePixelRatioF();
}

void Canavar::Engine::OpenGLWidget::initializeGL()
{
    qDebug() << "OpenGLWidget::initializeGL: QSurfaceFormat:" << QSurfaceFormat::defaultFormat();

    initializeOpenGLFunctions(); // Initialize OpenGL functions for the current context

    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    mPreviousTime = mCurrentTime;

    emit Initialized();

    CheckGLError();

    qDebug() << "OpenGLWidget::initializeGL: OpenGL version:" << reinterpret_cast<const char *>(glGetString(GL_VERSION));
}

void Canavar::Engine::OpenGLWidget::resizeGL(int Width, int Height)
{
    emit Resized(Width, Height);

    CheckGLError();
}

void Canavar::Engine::OpenGLWidget::paintGL()
{
    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    const float ifps = (mCurrentTime - mPreviousTime) * 0.001f;
    mPreviousTime = mCurrentTime;

    emit Render(ifps);

    CheckGLError();
}

void Canavar::Engine::OpenGLWidget::keyPressEvent(QKeyEvent *pEvent)
{
    emit KeyPressed(pEvent);
}

void Canavar::Engine::OpenGLWidget::keyReleaseEvent(QKeyEvent *pEvent)
{
    emit KeyReleased(pEvent);
}

void Canavar::Engine::OpenGLWidget::mousePressEvent(QMouseEvent *pEvent)
{
    emit MousePressed(pEvent);
}

void Canavar::Engine::OpenGLWidget::mouseReleaseEvent(QMouseEvent *pEvent)
{
    emit MouseReleased(pEvent);
}

void Canavar::Engine::OpenGLWidget::mouseMoveEvent(QMouseEvent *pEvent)
{
    emit MouseMoved(pEvent);
}

void Canavar::Engine::OpenGLWidget::wheelEvent(QWheelEvent *pEvent)
{
    emit WheelMoved(pEvent);
}

void Canavar::Engine::OpenGLWidget::closeEvent(QCloseEvent *pEvent)
{
    QOpenGLWidget::closeEvent(pEvent);

    emit WindowClosed();
}

void Canavar::Engine::OpenGLWidget::leaveEvent(QEvent *pEvent)
{
    emit LeaveEvent(pEvent);
}

void Canavar::Engine::OpenGLWidget::CheckGLError()
{
    GLenum Error;
    while ((Error = glGetError()) != GL_NO_ERROR) // Loop until all OpenGL errors are retrieved
    {
        CGE_EXIT_FAILURE("OpenGLWidget::CheckGLError: OpenGL error detected: '{}'", Canavar::Engine::Util::GetGlErrorString(Error));
    }
}
