#include "FreeCamera.h"

#include "Canavar/Engine/Util/Logger.h"
#include "Canavar/Engine/Node/NodeVisitor.h"

#include <QApplication>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTimer>
#include <QWheelEvent>
#include <QtMath>

Canavar::Engine::FreeCamera::FreeCamera()
{
    SetNodeName("Free Camera");
    mAnimator = std::make_shared<FreeCameraSlerpAnimator>(this);

    connect(mAnimator.get(), &FreeCameraAnimator::AnimationFinished, this, [this]() { Reset(); });
}

void Canavar::Engine::FreeCamera::Accept(NodeVisitor& visitor)
{
    visitor.Visit(*this);
}

void Canavar::Engine::FreeCamera::Update(float ifps)
{
    if (mAnimator->IsAnimating())
    {
        mAnimator->Update(ifps);
        return;
    }

    float dx = mAngularSpeedSmoothness * mMouse.DX;
    float dy = mAngularSpeedSmoothness * mMouse.DY;

    // Rotation
    if (mUpdateRotation)
    {
        RotateGlobal(QVector3D(0, 1, 0), mAngularSpeed * dx * ifps);
        RotateLocal(QVector3D(1, 0, 0), mAngularSpeed * dy * ifps);
        mMouse.DX -= dx;
        mMouse.DY -= dy;
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
                const auto delta = mLinearSpeed * ifps * direction;
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

bool Canavar::Engine::FreeCamera::KeyPressed(QKeyEvent* event)
{
    mPressedKeys.insert((Qt::Key) event->key(), true);
    mUpdatePosition = true;
    return true;
}

bool Canavar::Engine::FreeCamera::KeyReleased(QKeyEvent* event)
{
    mPressedKeys.insert((Qt::Key) event->key(), false);
    return false;
}

bool Canavar::Engine::FreeCamera::MousePressed(QMouseEvent* event)
{
    mMouse.X = event->position().x();
    mMouse.Y = event->position().y();
    mMouse.Button = event->button();

    return mMouse.Button == mActionReceiveButton;
}

bool Canavar::Engine::FreeCamera::MouseReleased(QMouseEvent* event)
{
    if (mMouse.Button == event->button())
    {
        mMouse.Button = Qt::NoButton;
    }

    return false;
}

bool Canavar::Engine::FreeCamera::MouseMoved(QMouseEvent* event)
{
    if (mMouse.Button == mActionReceiveButton)
    {
        mMouse.DX += mMouse.X - event->position().x();
        mMouse.DY += mMouse.Y - event->position().y();

        mMouse.X = event->position().x();
        mMouse.Y = event->position().y();
        mUpdateRotation = true;
        return true;
    }

    return false;
}

void Canavar::Engine::FreeCamera::GoToObject(Object* pNode)
{
    mAnimator->AnimateTo(pNode->GetWorldPosition());
}

void Canavar::Engine::FreeCamera::ToJson(QJsonObject& object)
{
    PerspectiveCamera::ToJson(object);

    object.insert("angular_speed", mAngularSpeed);
    object.insert("linear_speed", mLinearSpeed);
    object.insert("action_receive_button", mActionReceiveButton);
}

void Canavar::Engine::FreeCamera::FromJson(const QJsonObject& object, const QSet<NodePtr>& nodes)
{
    PerspectiveCamera::FromJson(object, nodes);

    mAngularSpeed = object["angular_speed"].toDouble(25.0f);
    mLinearSpeed = object["linear_speed"].toDouble(5.0f);
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
