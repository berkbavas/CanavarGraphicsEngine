#include "FreeCamera.h"

#include "Canavar/Engine/Util/Logger.h"

#include <QApplication>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTimer>
#include <QWheelEvent>
#include <QtMath>

Canavar::Engine::FreeCamera::FreeCamera()
{
    SetNodeName("Free Camera");
}

void Canavar::Engine::FreeCamera::Update(float ifps)
{
    // Rotation
    if (mUpdateRotation)
    {
        RotateGlobal(QVector3D(0, 1, 0), mAngularSpeedMultiplier * mAngularSpeed * mMouse.dx * ifps);
        RotateLocal(QVector3D(1, 0, 0), mAngularSpeedMultiplier * mAngularSpeed * mMouse.dy * ifps);
        mMouse.dx = 0.0f;
        mMouse.dy = 0.0f;
        mUpdateRotation = false;
    }

    // Translation
    if (mUpdatePosition)
    {
        if (mPressedKeys[Qt::Key_Space])
        {
            mLinearSpeed = 1000.0f;
        }
        else if (mPressedKeys[Qt::Key_Shift])
        {
            mLinearSpeed = 100.0f;
        }
        else if (mPressedKeys[Qt::Key_Control])
        {
            mLinearSpeed = 0.25f;
        }
        else
        {
            mLinearSpeed = 5.0f;
        }

        const auto keys = mPressedKeys.keys();

        for (const auto key : keys)
        {
            if (mPressedKeys.value(key, false))
            {
                const auto binding = KEY_BINDINGS.value(key, QVector3D(0, 0, 0));
                const auto direction = GetRotation().rotatedVector(binding);
                const auto delta = mLinearSpeed * mLinearSpeedMultiplier * ifps * direction;
                Translate(delta);
            }
        }
    }

    if (mPressedKeys.empty())
    {
        mUpdatePosition = false;
    }
}

void Canavar::Engine::FreeCamera::Reset()
{
    const auto keys = mPressedKeys.keys();

    for (const auto key : keys)
    {
        mPressedKeys.insert(key, false);
    }

    mUpdatePosition = false;
    mUpdateRotation = false;
    mMouse.Reset();
}

void Canavar::Engine::FreeCamera::KeyPressed(QKeyEvent* event)
{
    mPressedKeys.insert((Qt::Key) event->key(), true);
    mUpdatePosition = true;
}

void Canavar::Engine::FreeCamera::KeyReleased(QKeyEvent* event)
{
    mPressedKeys.insert((Qt::Key) event->key(), false);
}

void Canavar::Engine::FreeCamera::MousePressed(QMouseEvent* event)
{
    mMouse.x = event->position().x();
    mMouse.y = event->position().y();
    mMouse.button = event->button();
}

void Canavar::Engine::FreeCamera::MouseReleased(QMouseEvent* event)
{
    if (mMouse.button == event->button())
    {
        mMouse.button = Qt::NoButton;
    }
}

void Canavar::Engine::FreeCamera::MouseMoved(QMouseEvent* event)
{
    if (mMouse.button == mActionReceiveButton)
    {
        mMouse.dx += mMouse.x - event->position().x();
        mMouse.dy += mMouse.y - event->position().y();

        mMouse.x = event->position().x();
        mMouse.y = event->position().y();
        mUpdateRotation = true;
    }
}

void Canavar::Engine::FreeCamera::GoToObject(ObjectPtr pNode)
{
    SetWorldPosition(pNode->GetWorldPosition());
}

void Canavar::Engine::FreeCamera::ToJson(QJsonObject& object)
{
    PerspectiveCamera::ToJson(object);

    object.insert("angular_speed", mAngularSpeed);
    object.insert("linear_speed", mLinearSpeed);
    object.insert("linear_speed_multiplier", mLinearSpeedMultiplier);
    object.insert("angular_speed_muliplier", mAngularSpeedMultiplier);
    object.insert("action_receive_button", mActionReceiveButton);
}

void Canavar::Engine::FreeCamera::FromJson(const QJsonObject& object, const QSet<NodePtr>& nodes)
{
    PerspectiveCamera::FromJson(object, nodes);

    mAngularSpeed = object["angular_speed"].toDouble(25.0f);
    mLinearSpeed = object["linear_speed"].toDouble(5.0f);
    mLinearSpeedMultiplier = object["linear_speed_multiplier"].toDouble(1.0f);
    mAngularSpeedMultiplier = object["angular_speed_muliplier"].toDouble(1.0f);
    mActionReceiveButton = (Qt::MouseButton) object["action_receive_button"].toInt(Qt::MiddleButton);
}

const QMap<Qt::Key, QVector3D> Canavar::Engine::FreeCamera::KEY_BINDINGS = //
    {
        { Qt::Key_W, QVector3D(0, 0, -1) }, //
        { Qt::Key_S, QVector3D(0, 0, 1) },  //
        { Qt::Key_A, QVector3D(-1, 0, 0) }, //
        { Qt::Key_D, QVector3D(1, 0, 0) },  //
        { Qt::Key_E, QVector3D(0, 1, 0) },  //
        { Qt::Key_Q, QVector3D(0, -1, 0) }  //
    };
