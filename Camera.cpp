#include "Camera.h"

#include <QTransform>

Camera::Camera(QObject *parent)
    : QObject(parent)
    , mMovementSpeed(0.0125f)
    , mAngularSpeed(0.125f)
    , mMousePressed(false)
    , mMousePreviousX(0.0f)
    , mMousePreviousY(0.0f)
    , mMouseDeltaX(0.0f)
    , mMouseDeltaY(0.0f)
{
    mTransformation.setToIdentity();
    mProjection.setToIdentity();

    connect(&mTimer, &QTimer::timeout, this, &Camera::update);
    mTimer.start(5);
}

void Camera::onKeyPressed(QKeyEvent *event)
{
    mPressedKeys.insert((Qt::Key) event->key(), true);
}

void Camera::onKeyReleased(QKeyEvent *event)
{
    mPressedKeys.insert((Qt::Key) event->key(), false);
}

void Camera::onMousePressed(QMouseEvent *event)
{
    mMousePreviousX = event->x();
    mMousePreviousY = event->y();
    mMousePressed = true;
}

void Camera::onMouseReleased(QMouseEvent *)
{
    mMousePressed = false;
}

void Camera::onMouseMoved(QMouseEvent *event)
{
    if (mMousePressed)
    {
        mMouseDeltaX += mMousePreviousX - event->x();
        mMouseDeltaY += mMousePreviousY - event->y();

        mMousePreviousX = event->x();
        mMousePreviousY = event->y();
    }
}

void Camera::update()
{
    mTransformation.setToIdentity();

    // Rotation
    {
        mRotation = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), mAngularSpeed * mMouseDeltaX) * mRotation;
        mRotation = mRotation * QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), mAngularSpeed * mMouseDeltaY);

        mMouseDeltaY = 0.0f;
        mMouseDeltaX = 0.0f;

        mTransformation.rotate(mRotation.conjugated());
    }

    // Translation
    {
        const QList<Qt::Key> keys = mPressedKeys.keys();

        if (mPressedKeys[Qt::Key_Shift])
            mMovementSpeed = 0.25f;
        else if (mPressedKeys[Qt::Key_Control])
            mMovementSpeed = 0.0125f;
        else
            mMovementSpeed = 0.05f;

        for (auto key : keys)
            if (mPressedKeys.value(key, false))
                mPosition -= mMovementSpeed * mRotation.rotatedVector(KEY_BINDINGS.value(key, QVector3D(0, 0, 0)));

        mTransformation.translate(mPosition);
    }
}

const QMatrix4x4 &Camera::projection() const
{
    return mProjection;
}

void Camera::setProjection(const QMatrix4x4 &newProjection)
{
    mProjection = newProjection;
}

QVector3D Camera::position() const
{
    return -1 * mPosition;
}

void Camera::setPosition(const QVector3D &newPosition)
{
    mPosition = -1 * newPosition;
}

QQuaternion Camera::rotation() const
{
    return mRotation.conjugated();
}

void Camera::setRotation(const QQuaternion &newRotation)
{
    mRotation = newRotation.conjugated();
}

const QMatrix4x4 &Camera::transformation()
{
    return mTransformation;
}

const QMap<Qt::Key, QVector3D> Camera::KEY_BINDINGS = {
    {Qt::Key_W, QVector3D(0, 0, -1)},
    {Qt::Key_S, QVector3D(0, 0, 1)},
    {Qt::Key_A, QVector3D(-1, 0, 0)},
    {Qt::Key_D, QVector3D(1, 0, 0)},
    {Qt::Key_E, QVector3D(0, 1, 0)},
    {Qt::Key_Q, QVector3D(0, -1, 0)},
};
