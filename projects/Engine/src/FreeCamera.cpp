#include "FreeCamera.h"

Canavar::Engine::FreeCamera::FreeCamera()
    : PerspectiveCamera()
    , mUpdateRotation(true)
    , mUpdatePosition(true)
{
    mType = Node::NodeType::FreeCamera;

    connect(this, &Canavar::Engine::Camera::ActiveChanged, this, [=]() {
        if (!mActive)
            Reset();
        });
}

void Canavar::Engine::FreeCamera::MouseDoubleClicked(QMouseEvent*) {}

void Canavar::Engine::FreeCamera::WheelMoved(QWheelEvent*) {}

void Canavar::Engine::FreeCamera::MousePressed(QMouseEvent* event)
{
    mMouse.mX = event->position().x();
    mMouse.mY = event->position().y();
    mMouse.mPressedButton = event->button();
}

void Canavar::Engine::FreeCamera::MouseReleased(QMouseEvent* event)
{
    if (mMouse.mPressedButton == event->button())
        mMouse.mPressedButton = Qt::NoButton;
}

void Canavar::Engine::FreeCamera::MouseMoved(QMouseEvent* event)
{
    if (mMouse.mPressedButton == Qt::MiddleButton)
    {
        mMouse.mDx += mMouse.mX - event->position().x();
        mMouse.mDy += mMouse.mY - event->position().y();

        mMouse.mX = event->position().x();
        mMouse.mY = event->position().y();
        mUpdateRotation = true; 
    }
}

void Canavar::Engine::FreeCamera::KeyPressed(QKeyEvent* event)
{
    mPressedKeys.insert((Qt::Key)event->key(), true);
    mUpdatePosition = true;
}

void Canavar::Engine::FreeCamera::KeyReleased(QKeyEvent* event)
{
    mPressedKeys.insert((Qt::Key)event->key(), false);
}

void Canavar::Engine::FreeCamera::Update(float ifps)
{
    // Rotation
    if (mUpdateRotation)
    {
        auto newRotation = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), mSpeed.mAngularMultiplier * mSpeed.mAngular * mMouse.mDx * ifps) * WorldRotation();
        newRotation = newRotation * QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), mSpeed.mAngularMultiplier * mSpeed.mAngular * mMouse.mDy * ifps);
        SetWorldRotation(newRotation);

        mMouse.mDx = 0.0f;
        mMouse.mDy = 0.0f;
        mUpdateRotation = false;
    }

    // Translation
    if (mUpdatePosition)
    {
        const QList<Qt::Key> keys = mPressedKeys.keys();

        if (mPressedKeys[Qt::Key_Control])
            mSpeed.mLinear = 1000.0f;
        else if (mPressedKeys[Qt::Key_Shift])
            mSpeed.mLinear = 100.0f;
        else
            mSpeed.mLinear = 10.0f;

        for (auto key : keys)
            if (mPressedKeys.value(key, false))
                SetWorldPosition(WorldPosition() + mSpeed.mLinear * mSpeed.mLinearMultiplier * ifps * mRotation.rotatedVector(KEY_BINDINGS.value(key, QVector3D(0, 0, 0))));
    }

    if (mPressedKeys.empty())
        mUpdatePosition = false;
}

void Canavar::Engine::FreeCamera::Reset()
{
    auto keys = mPressedKeys.keys();
    for (auto key : qAsConst(keys))
        mPressedKeys.insert(key, false);

    mUpdatePosition = false;
    mUpdateRotation = false;

    mMouse.Reset();
}

const QMap<Qt::Key, QVector3D> Canavar::Engine::FreeCamera::KEY_BINDINGS = {
    {Qt::Key_W, QVector3D(0, 0, -1)},
    {Qt::Key_S, QVector3D(0, 0, 1)},
    {Qt::Key_A, QVector3D(-1, 0, 0)},
    {Qt::Key_D, QVector3D(1, 0, 0)},
    {Qt::Key_E, QVector3D(0, 1, 0)},
    {Qt::Key_Q, QVector3D(0, -1, 0)},
};