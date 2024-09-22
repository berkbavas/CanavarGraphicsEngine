#include "PersecutorCamera.h"

Canavar::Engine::PersecutorCamera::PersecutorCamera()
    : PerspectiveCamera()
    , mTarget(nullptr)
    , mDistance(10.0f)
    , mYaw(0.0f)
    , mPitch(0.0f)
{
    mType = Node::NodeType::PersecutorCamera;

    connect(this, &Camera::ActiveChanged, this, [=]() { Update(0); });
}

void Canavar::Engine::PersecutorCamera::MouseDoubleClicked(QMouseEvent*) {}

void Canavar::Engine::PersecutorCamera::MousePressed(QMouseEvent* event)
{
    mMouse.mX = event->position().x();
    mMouse.mY = event->position().y();
    mMouse.mPressedButton = event->button();
}

void Canavar::Engine::PersecutorCamera::MouseReleased(QMouseEvent* event)
{
    if (mMouse.mPressedButton == event->button())
        mMouse.Reset();
}

void Canavar::Engine::PersecutorCamera::MouseMoved(QMouseEvent* event)
{
    if (mMouse.mPressedButton == Qt::MiddleButton)
    {
        mMouse.mDx += mMouse.mX - event->position().x();
        mMouse.mDy += mMouse.mY - event->position().y();

        mMouse.mX = event->position().x();
        mMouse.mY = event->position().y();
    }
}
void Canavar::Engine::PersecutorCamera::WheelMoved(QWheelEvent* event)
{
    if (event->angleDelta().y() < 0)
        mDistance += 0.5;

    if (event->angleDelta().y() > 0)
        mDistance -= 0.5;

    mDistance = qBound(0.1f, mDistance, 100.0f);
}

void Canavar::Engine::PersecutorCamera::KeyPressed(QKeyEvent*) {}

void Canavar::Engine::PersecutorCamera::KeyReleased(QKeyEvent*) {}

void Canavar::Engine::PersecutorCamera::Update(float ifps)
{
    if (mTarget)
    {
        mYaw += mSpeed.mAngularMultiplier * mSpeed.mAngular * mMouse.mDx * ifps;
        mPitch += mSpeed.mAngularMultiplier * mSpeed.mAngular * mMouse.mDy * ifps;

        auto newRotation = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), mYaw) * QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), mPitch);
        auto newWorldPosition = mTarget->WorldPosition() + mDistance * newRotation * QVector3D(0, 0, 1);

        SetWorldPosition(newWorldPosition);
        SetWorldRotation(newRotation);

        mMouse.mDx = 0.0f;
        mMouse.mDy = 0.0f;
    }
}

void Canavar::Engine::PersecutorCamera::Reset()
{
    mDistance = 10.0f;
    mYaw = 0.0f;
    mPitch = 0.0f;
}

Canavar::Engine::Node* Canavar::Engine::PersecutorCamera::GetTarget() const
{
    return mTarget;
}

void Canavar::Engine::PersecutorCamera::SetTarget(Node* newTarget)
{
    if (mTarget)
        mTarget->disconnect(this);

    mTarget = newTarget;

    if (mTarget)
        connect(mTarget, &QObject::destroyed, this, [=]() { mTarget = nullptr; });

    Reset();
}