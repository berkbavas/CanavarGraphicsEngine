#include "OrthographicCamera.h"

bool Canavar::Engine::OrthographicCamera::MousePressed(QMouseEvent* event)
{
    mMouse.X = event->position().x();
    mMouse.Y = event->position().y();
    mMouse.DX = 0;
    mMouse.DY = 0;
    mMouse.Button = event->button();
    return true;
}

bool Canavar::Engine::OrthographicCamera::MouseReleased(QMouseEvent*)
{
    mMouse.Button = Qt::NoButton;
    return true;
}

bool Canavar::Engine::OrthographicCamera::MouseMoved(QMouseEvent* event)
{
    if (mMouse.Button == mActionReceiveButton)
    {
        mMouse.DX += mMouse.X - event->pos().x();
        mMouse.DY += mMouse.Y - event->pos().y();

        mMouse.X = event->pos().x();
        mMouse.Y = event->pos().y();

        mLeft += mDevicePixelRatio * mZoom * mMouse.DX;
        mTop += mDevicePixelRatio * mZoom * mMouse.DY;
        mMouse.DX = 0;
        mMouse.DY = 0;
        return true;
    }

    return false;
}

bool Canavar::Engine::OrthographicCamera::WheelMoved(QWheelEvent* event)
{
    QPointF cursorWorldPosition = CameraToWorld(event->position());

    if (event->angleDelta().y() < 0)
        mZoom = 1.1f * mZoom;
    else
        mZoom = mZoom / 1.1f;

    mZoom = qMax(0.001f, qMin(10.0f, mZoom));

    QPointF newWorldPosition = CameraToWorld(event->position());
    QPointF delta = cursorWorldPosition - newWorldPosition;
    mLeft += delta.x();
    mTop += delta.y();

    return true;
}

QMatrix4x4 Canavar::Engine::OrthographicCamera::GetProjectionMatrix() const
{
    QMatrix4x4 ProjectionMatrix;
    ProjectionMatrix.ortho(mLeft, //
                           mLeft + mWidth * mZoom,
                           mTop + mHeight * mZoom,
                           mTop,
                           mZNear,
                           mZFar);
    return ProjectionMatrix;
}

QMatrix4x4 Canavar::Engine::OrthographicCamera::GetViewProjectionMatrix() const
{
    return GetProjectionMatrix() * GetViewMatrix();
}

QMatrix4x4 Canavar::Engine::OrthographicCamera::GetRotationMatrix() const
{
    constexpr QVector4D ZERO_TRANSLATION(0, 0, 0, 1);
    QMatrix4x4 ViewMatrix = GetViewMatrix();
    ViewMatrix.setColumn(3, ZERO_TRANSLATION);
    return ViewMatrix;
}

QMatrix4x4 Canavar::Engine::OrthographicCamera::GetViewMatrix() const
{
    QMatrix4x4 ViewMatrix;
    ViewMatrix.rotate(GetWorldRotation().conjugated());
    ViewMatrix.translate(-GetWorldPosition());
    return ViewMatrix;
}

QVector3D Canavar::Engine::OrthographicCamera::GetViewDirection() const
{
    constexpr QVector3D NEGATIVE_Z(0, 0, -1);
    return GetWorldRotation() * NEGATIVE_Z;
}

float Canavar::Engine::OrthographicCamera::GetAspectRatio() const
{
    return mWidth / mHeight;
}

QVector2D Canavar::Engine::OrthographicCamera::CameraToWorld(const QVector2D& camera)
{
    return CameraToWorld(camera.x(), camera.y());
}

QPointF Canavar::Engine::OrthographicCamera::CameraToWorld(const QPointF& camera)
{
    return CameraToWorld(camera.x(), camera.y()).toPointF();
}

QVector2D Canavar::Engine::OrthographicCamera::CameraToWorld(float x, float y)
{
    return QVector2D(mLeft + x * mZoom * mDevicePixelRatio, mTop + y * mZoom * mDevicePixelRatio);
}

QVector2D Canavar::Engine::OrthographicCamera::WorldToCamera(const QVector2D& world)
{
    return WorldToCamera(world.x(), world.y());
}

QPointF Canavar::Engine::OrthographicCamera::WorldToCamera(const QPointF& world)
{
    return WorldToCamera(world.x(), world.y()).toPointF();
}

float Canavar::Engine::OrthographicCamera::WorldDistanceToCameraDistance(float distance)
{
    const auto origin = WorldToCamera(QPointF(0, 0));
    const auto vector = WorldToCamera(QPointF(0, distance));
    const auto delta = vector - origin;

    return std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());
}

float Canavar::Engine::OrthographicCamera::CameraDistanceToWorldDistance(float distance)
{
    const auto origin = CameraToWorld(QPointF(0, 0));
    const auto vector = CameraToWorld(QPointF(0, distance));
    const auto delta = vector - origin;

    return std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());
}

void Canavar::Engine::OrthographicCamera::Reset()
{
    mZoom = 1.0f;
    mTop = 0.0f;
    mLeft = 0.0f;
}

QVector2D Canavar::Engine::OrthographicCamera::WorldToCamera(float x, float y)
{
    x -= mLeft;
    y -= mTop;

    return QVector2D(x / mZoom, y / mZoom) / mDevicePixelRatio;
}
