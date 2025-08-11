#include "PersecutorCamera.h"

#include "Canavar/Engine/Util/Logger.h"

Canavar::Engine::PersecutorCamera::PersecutorCamera()
{
    SetNodeName("Persecutor Camera");

    mAnimator = std::make_shared<PersecutorCameraNonlinearAnimator>();

    connect(mAnimator.get(), &PersecutorCameraAnimator::Updated, this, &PersecutorCamera::OnAnimationAnglesUpdated);
}

bool Canavar::Engine::PersecutorCamera::MousePressed(QMouseEvent* event)
{
    if (ShouldIgnoreEvents())
    {
        return false;
    }

    mMouse.x = event->position().x() * mDevicePixelRatio;
    mMouse.y = event->position().y() * mDevicePixelRatio;
    mMouse.z = event->position().x() * mDevicePixelRatio;
    mMouse.w = event->position().y() * mDevicePixelRatio;
    mMouse.button = event->button();

    return true;
}

bool Canavar::Engine::PersecutorCamera::MouseReleased(QMouseEvent* event)
{
    if (ShouldIgnoreEvents())
    {
        return false;
    }

    if (mMouse.button == event->button())
    {
        mMouse.button = Qt::NoButton;
    }

    return false;
}

bool Canavar::Engine::PersecutorCamera::MouseMoved(QMouseEvent* event)
{
    if (ShouldIgnoreEvents())
    {
        return false;
    }

    float x = event->position().x() * mDevicePixelRatio;
    float y = event->position().y() * mDevicePixelRatio;

    if (mMouse.button == Qt::MiddleButton)
    {
        mMouse.dx += mMouse.x - x;
        mMouse.dy += mMouse.y - y;

        mMouse.x = x;
        mMouse.y = y;

        return true;
    }
    else if (mMouse.button == Qt::RightButton)
    {
        mMouse.dz += mMouse.z - x;
        mMouse.dw += mMouse.w - y;

        mMouse.z = x;
        mMouse.w = y;

        return true;
    }

    return false;
}

bool Canavar::Engine::PersecutorCamera::WheelMoved(QWheelEvent* event)
{
    if (ShouldIgnoreEvents())
    {
        return false;
    }

    if (event->angleDelta().y() < 0)
    {
        mDistanceBuffer += 0.5;
    }

    if (event->angleDelta().y() > 0)
    {
        mDistanceBuffer -= 0.5;
    }

    return true;
}

void Canavar::Engine::PersecutorCamera::Update(float ifps)
{
    if (mAnimator->IsAnimating())
    {
        mAnimator->Update(ifps);
    }

    QVector3D TargetPosition(0, 0, 0);

    if (mTarget)
    {
        TargetPosition = mTarget->GetWorldPosition();
    }

    float dx = mAngularSpeedSmoothness * mMouse.dx;
    float dy = mAngularSpeedSmoothness * mMouse.dy;

    float dz = mMouse.dz;
    float dw = mMouse.dw;

    if (dx != 0 || dy != 0)
    {
        mYaw += mAngularSpeed * dx * ifps;
        mPitch += mAngularSpeed * dy * ifps;
    }

    if (dz != 0 || dw != 0)
    {
        constexpr QVector3D DOWN_DIR(0, -1, 0);
        constexpr QVector3D LEFT_DIR(1, 0, 0);

        mTranslation += (mLinearSpeed * dz * ifps) * (GetRotation() * LEFT_DIR); // (...) are important
        mTranslation += (mLinearSpeed * dw * ifps) * (GetRotation() * DOWN_DIR);
    }

    ClampAngles();

    if (abs(mDistanceBuffer) < mZoomSmoothness)
    {
        mDistanceBuffer = 0;
    }

    mDistance += mDistanceBuffer * mZoomSmoothness;
    mDistanceBuffer -= mDistanceBuffer * mZoomSmoothness;

    mDistance = qBound(0.1f, mDistance, 100.0f);

    auto newRotation = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), mYaw) * QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), mPitch);
    auto newWorldPosition = TargetPosition + mTranslation + mDistance * newRotation * QVector3D(0, 0, 1);

    SetWorldPosition(newWorldPosition);
    SetWorldRotation(newRotation);

    mMouse.dx -= dx;
    mMouse.dy -= dy;

    mMouse.dz -= dz;
    mMouse.dw -= dw;

    if (abs(mMouse.dx) < mAngularSpeedSmoothness)
    {
        mMouse.dx = 0;
    }

    if (abs(mMouse.dy) < mAngularSpeedSmoothness)
    {
        mMouse.dy = 0;
    }
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
        object.insert("target_uuid", QString::fromStdString(mTarget->GetUuid()));
    }

    object.insert("angular_speed", mAngularSpeed);
}

void Canavar::Engine::PersecutorCamera::FromJson(const QJsonObject& object, const QSet<NodePtr>& nodes)
{
    PerspectiveCamera::FromJson(object, nodes);

    mAngularSpeed = object["angular_speed"].toDouble(25.0f);

    QString targetUuid = object["target_uuid"].toString("");

    if (targetUuid.isEmpty() == false)
    {
        LOG_DEBUG("PersecutorCamera::FromJson: UUID of my target is {}.", targetUuid.toStdString());

        for (const auto pNode : nodes)
        {
            if (targetUuid == pNode->GetUuid())
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
