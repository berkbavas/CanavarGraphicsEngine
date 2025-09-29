#include "OrthographicCamera.h"

bool Canavar::Engine::OrthographicCamera::MousePressed(QMouseEvent* pEvent)
{
    mMouse.X = pEvent->position().x();
    mMouse.Y = pEvent->position().y();
    mMouse.DX = 0;
    mMouse.DY = 0;
    mMouse.Button = pEvent->button();
    return true;
}

bool Canavar::Engine::OrthographicCamera::MouseReleased(QMouseEvent*)
{
    mMouse.Button = Qt::NoButton;
    return true;
}

bool Canavar::Engine::OrthographicCamera::MouseMoved(QMouseEvent* pEvent)
{
    if (mMouse.Button == mActionReceiveButton)
    {
        mMouse.DX += mMouse.X - pEvent->pos().x();
        mMouse.DY += mMouse.Y - pEvent->pos().y();

        mMouse.X = pEvent->pos().x();
        mMouse.Y = pEvent->pos().y();

        mLeft += mDevicePixelRatio * mZoom * mMouse.DX;
        mTop += mDevicePixelRatio * mZoom * mMouse.DY;
        mMouse.DX = 0;
        mMouse.DY = 0;
        return true;
    }

    return false;
}

bool Canavar::Engine::OrthographicCamera::WheelMoved(QWheelEvent* pEvent)
{
    QPointF CursorWorldPosition = CameraToWorld(pEvent->position());

    if (pEvent->angleDelta().y() < 0)
        mZoom = 1.1f * mZoom;
    else
        mZoom = mZoom / 1.1f;

    mZoom = qMax(0.001f, qMin(10.0f, mZoom));

    QPointF NewWorldPosition = CameraToWorld(pEvent->position());
    QPointF Delta = CursorWorldPosition - NewWorldPosition;
    mLeft += Delta.x();
    mTop += Delta.y();

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

float Canavar::Engine::OrthographicCamera::WorldDistanceToCameraDistance(float Distance)
{
    const auto Origin = WorldToCamera(QPointF(0, 0));
    const auto Vector = WorldToCamera(QPointF(0, Distance));
    const auto Delta = Vector - Origin;

    return std::sqrt(Delta.x() * Delta.x() + Delta.y() * Delta.y());
}

float Canavar::Engine::OrthographicCamera::CameraDistanceToWorldDistance(float Distance)
{
    const auto Origin = CameraToWorld(QPointF(0, 0));
    const auto Vector = CameraToWorld(QPointF(0, Distance));
    const auto Delta = Vector - Origin;

    return std::sqrt(Delta.x() * Delta.x() + Delta.y() * Delta.y());
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
