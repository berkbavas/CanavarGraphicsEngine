#include "PersecutorCamera.h"

#include "Canavar/Engine/Util/Logger.h"

Canavar::Engine::PersecutorCamera::PersecutorCamera()
{
    SetNodeName("Persecutor Camera");

    mAnimator = std::make_shared<PersecutorCameraNonlinearAnimator>();

    connect(mAnimator.get(), &PersecutorCameraAnimator::Updated, this, &PersecutorCamera::OnAnimationAnglesUpdated);
}

void Canavar::Engine::PersecutorCamera::MousePressed(QMouseEvent* event)
{
    if (ShouldIgnoreEvents())
    {
        return;
    }

    mMouse.x = event->position().x();
    mMouse.y = event->position().y();
    mMouse.button = event->button();
}

void Canavar::Engine::PersecutorCamera::MouseReleased(QMouseEvent* event)
{
    if (ShouldIgnoreEvents())
    {
        return;
    }

    if (mMouse.button == event->button())
    {
        mMouse.Reset();
    }
}

void Canavar::Engine::PersecutorCamera::MouseMoved(QMouseEvent* event)
{
    if (ShouldIgnoreEvents())
    {
        return;
    }

    if (mMouse.button == Qt::MiddleButton || mMouse.button == Qt::RightButton)
    {
        mMouse.dx += mMouse.x - event->position().x();
        mMouse.dy += mMouse.y - event->position().y();

        mMouse.x = event->position().x();
        mMouse.y = event->position().y();
    }
}
void Canavar::Engine::PersecutorCamera::WheelMoved(QWheelEvent* event)
{
    if (ShouldIgnoreEvents())
    {
        return;
    }

    if (event->angleDelta().y() < 0)
        mDistance += 0.1;

    if (event->angleDelta().y() > 0)
        mDistance -= 0.1;

    mDistance = qBound(0.1f, mDistance, 100.0f);
}

void Canavar::Engine::PersecutorCamera::Update(float ifps)
{
    if (mAnimator->IsAnimating())
    {
        mAnimator->Update(ifps);
    }

    QVector3D targetPos(0, 0, 0);

    if (mTarget)
    {
        targetPos = mTarget->GetWorldPosition();
    }

    if (mMouse.button == Qt::MiddleButton)
    {
        mYaw += mAngularSpeedMultiplier * mAngularSpeed * mMouse.dx * ifps;
        mPitch += mAngularSpeedMultiplier * mAngularSpeed * mMouse.dy * ifps;
    }

    if (mMouse.button == Qt::RightButton)
    {
        constexpr QVector3D DOWN_DIR(0, -1, 0);
        constexpr QVector3D LEFT_DIR(1, 0, 0);

        mTranslation += (mLinearSpeed * mMouse.dx * ifps) * (GetRotation() * LEFT_DIR); // (...) are important
        mTranslation += (mLinearSpeed * mMouse.dy * ifps) * (GetRotation() * DOWN_DIR);
    }

    ClampAngles();

    auto newRotation = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), mYaw) * QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), mPitch);
    auto newWorldPosition = targetPos + mTranslation + mDistance * newRotation * QVector3D(0, 0, 1);

    SetWorldPosition(newWorldPosition);
    SetWorldRotation(newRotation);

    mMouse.dx = 0.0f;
    mMouse.dy = 0.0f;
}

void Canavar::Engine::PersecutorCamera::Reset()
{
    mDistance = 5.0f;
    mYaw = 0.0f;
    mPitch = 0.0f;
    mTranslation = QVector3D(0, 0, 0);
    mAnimator->Stop();
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

void Canavar::Engine::PersecutorCamera::AnimateTo(float yaw, float pitch)
{
    mMouse.Reset();
    mAnimator->Animate(mYaw, mPitch, yaw, pitch);
}

void Canavar::Engine::PersecutorCamera::AnimateTo(ViewDirection viewDirection)
{
    switch (viewDirection)
    {
    case ViewDirection::Front:
        AnimateTo(0, 0);
        break;
    case ViewDirection::Back:
        AnimateTo(180, 0);
        break;
    case ViewDirection::Up:
        AnimateTo(0, 270);
        break;
    case ViewDirection::Down:
        AnimateTo(0, 90);
        break;
    case ViewDirection::Left:
        AnimateTo(270, 0);
        break;
    case ViewDirection::Right:
        AnimateTo(90, 0);
        break;
    default:
        break;
    }
}

void Canavar::Engine::PersecutorCamera::OnAnimationAnglesUpdated(float yaw, float pitch)
{
    mYaw = yaw;
    mPitch = pitch;
}

void Canavar::Engine::PersecutorCamera::ClampAngles()
{
    if (mYaw < 0)
    {
        mYaw += 360;
    }

    if (mYaw > 360)
    {
        mYaw -= 360;
    }

    if (mPitch < 0)
    {
        mPitch += 360;
    }

    if (mPitch > 360)
    {
        mPitch -= 360;
    }
}
bool Canavar::Engine::PersecutorCamera::IsAnimating() const
{
    return mAnimator->IsAnimating();
}

bool Canavar::Engine::PersecutorCamera::ShouldIgnoreEvents() const
{
    return IsAnimating();
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
