#include "PersecutorCamera.h"

#include "Canavar/Engine/Util/Logger.h"

Canavar::Engine::PersecutorCamera::PersecutorCamera()
{
    SetNodeName("Persecutor Camera");
}

void Canavar::Engine::PersecutorCamera::MousePressed(QMouseEvent* event)
{
    mMouse.x = event->position().x();
    mMouse.y = event->position().y();
    mMouse.button = event->button();
}

void Canavar::Engine::PersecutorCamera::MouseReleased(QMouseEvent* event)
{
    if (mMouse.button == event->button())
    {
        mMouse.Reset();
    }
}

void Canavar::Engine::PersecutorCamera::MouseMoved(QMouseEvent* event)
{
    if (mMouse.button == Qt::MiddleButton)
    {
        mMouse.dx += mMouse.x - event->position().x();
        mMouse.dy += mMouse.y - event->position().y();

        mMouse.x = event->position().x();
        mMouse.y = event->position().y();
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

void Canavar::Engine::PersecutorCamera::Update(float ifps)
{
    if (mTarget)
    {
        mYaw += mAngularSpeedMultiplier * mAngularSpeed * mMouse.dx * ifps;
        mPitch += mAngularSpeedMultiplier * mAngularSpeed * mMouse.dy * ifps;

        auto newRotation = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), mYaw) * QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), mPitch);
        auto newWorldPosition = mTarget->GetWorldPosition() + mDistance * newRotation * QVector3D(0, 0, 1);

        SetWorldPosition(newWorldPosition);
        SetWorldRotation(newRotation);

        mMouse.dx = 0.0f;
        mMouse.dy = 0.0f;
    }
}

void Canavar::Engine::PersecutorCamera::Reset()
{
    mDistance = 5.0f;
    mYaw = 0.0f;
    mPitch = 0.0f;
}

Canavar::Engine::ObjectPtr Canavar::Engine::PersecutorCamera::GetTarget() const
{
    return mTarget;
}

void Canavar::Engine::PersecutorCamera::SetTarget(ObjectPtr pNewTarget)
{
    LOG_DEBUG("PersecutorCamera::SetTarget: mTarget: {}, pNewTarget: {}", PRINT_ADDRESS(mTarget.get()), PRINT_ADDRESS(pNewTarget.get()));

    if (pNewTarget == nullptr)
    {
        LOG_INFO("PersecutorCamera::SetTarget: pNewTarget is nullptr.");
    }

    mTarget = pNewTarget;
    Reset();
}

void Canavar::Engine::PersecutorCamera::ToJson(QJsonObject& object)
{
    PerspectiveCamera::ToJson(object);

    if (mTarget)
    {
        object.insert("target", mTarget->GetUuid());
    }

    object.insert("angular_speed", mAngularSpeed);
    object.insert("angular_speed_muliplier", mAngularSpeedMultiplier);
}

void Canavar::Engine::PersecutorCamera::FromJson(const QJsonObject& object, const std::map<QString, NodePtr>& nodes)
{
    PerspectiveCamera::FromJson(object, nodes);

    mAngularSpeed = object["angular_speed"].toDouble(25.0f);
    mAngularSpeedMultiplier = object["angular_speed_muliplier"].toDouble(1.0f);

    QString target = object["target"].toString("");

    if (target.isEmpty() == false)
    {
        LOG_DEBUG("PersecutorCamera::FromJson: UUID of my target is {}.", target.toStdString());

        for (const auto& [uuid, pNode] : nodes)
        {
            if (target == uuid)
            {
                LOG_DEBUG("PersecutorCamera::FromJson: I have found my target.");
                SetTarget(std::dynamic_pointer_cast<Object>(pNode));
            }
        }
    }
    else
    {
        LOG_DEBUG("PersecutorCamera::FromJson: I have no targets.");
    }
}

void Canavar::Engine::PersecutorCamera::SetParent(ObjectWeakPtr pParentNode)
{
    LOG_WARN("PersecutorCamera::SetParent: Cannot assign parent to PersecutorCamera");
}

void Canavar::Engine::PersecutorCamera::AddChild(ObjectPtr pNode)
{
    LOG_WARN("PersecutorCamera::AddChild: Cannot add child to PersecutorCamera.");
}

void Canavar::Engine::PersecutorCamera::RemoveChild(ObjectPtr pNode)
{
    LOG_WARN("PersecutorCamera::RemoveChild: PersecutorCamera cannot have any children.");
}
