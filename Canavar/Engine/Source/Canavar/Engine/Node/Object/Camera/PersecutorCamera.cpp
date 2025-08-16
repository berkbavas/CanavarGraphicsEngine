#include "PersecutorCamera.h"

#include "Canavar/Engine/Util/Logger.h"
#include "Canavar/Engine/Util/Math.h"

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

    mMouse.X = event->position().x();
    mMouse.Y = event->position().y();
    mMouse.Z = event->position().x();
    mMouse.W = event->position().y();
    mMouse.Button = event->button();

    return true;
}

bool Canavar::Engine::PersecutorCamera::MouseReleased(QMouseEvent* event)
{
    if (ShouldIgnoreEvents())
    {
        return false;
    }

    if (mMouse.Button == event->button())
    {
        mMouse.Button = Qt::NoButton;
    }

    return false;
}

bool Canavar::Engine::PersecutorCamera::MouseMoved(QMouseEvent* event)
{
    if (ShouldIgnoreEvents())
    {
        return false;
    }

    float x = event->position().x();
    float y = event->position().y();

    if (mMouse.Button == Qt::MiddleButton)
    {
        mMouse.DX += mMouse.X - x;
        mMouse.DY += mMouse.Y - y;

        mMouse.X = x;
        mMouse.Y = y;

        return true;
    }
    else if (mMouse.Button == Qt::RightButton)
    {
        mMouse.DZ += mMouse.Z - x;
        mMouse.DW += mMouse.W - y;

        mMouse.Z = x;
        mMouse.W = y;

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
        mDistanceBuffer += mZoomStep;
    }

    if (event->angleDelta().y() > 0)
    {
        mDistanceBuffer -= mZoomStep;
    }

    return true;
}

void Canavar::Engine::PersecutorCamera::Update(float ifps)
{
    if (mAnimator->IsAnimating())
    {
        mAnimator->Update(ifps);
    }

    HandleRotation(ifps);
    HandleZoom(ifps);
    HandleTranslation(ifps);

    QVector3D TargetPosition(0, 0, 0);

    if (mTarget)
    {
        TargetPosition = mTarget->GetWorldPosition();
    }

    auto NewRotation = QQuaternion::fromAxisAndAngle(POSITIVE_Y, mYaw) * QQuaternion::fromAxisAndAngle(POSITIVE_X, mPitch);
    auto NewWorldPosition = TargetPosition + mTranslation + mDistance * NewRotation * POSITIVE_Z;

    SetWorldPosition(NewWorldPosition);
    SetWorldRotation(NewRotation);
}

void Canavar::Engine::PersecutorCamera::HandleZoom(float ifps)
{
    if (abs(mDistanceBuffer) < 0.1f)
    {
        mDistanceBuffer = 0;
    }

    float Step = mDistanceBuffer * mZoomSmoothness * ifps;
    mDistance += Step;
    mDistanceBuffer -= Step;
    mDistance = qBound(MIN_DISTANCE, mDistance, MAX_DISTANCE);
}

void Canavar::Engine::PersecutorCamera::HandleRotation(float ifps)
{
    float StepX = mAngularSpeedSmoothness * mMouse.DX;
    float StepY = mAngularSpeedSmoothness * mMouse.DY;

    // Consume
    mMouse.DX -= StepX;
    mMouse.DY -= StepY;

    if (StepX != 0 || StepY != 0)
    {
        mYaw += mAngularSpeed * StepX * ifps;
        mPitch += mAngularSpeed * StepY * ifps;
    }

    ClampAngles();
}

void Canavar::Engine::PersecutorCamera::HandleTranslation(float ifps)
{
    float StepZ = mLinearSpeedSmoothness * mMouse.DZ;
    float StepW = mLinearSpeedSmoothness * mMouse.DW;

    // Consume
    mMouse.DZ -= StepZ;
    mMouse.DW -= StepW;

    if (StepZ != 0 || StepW != 0)
    {
        constexpr QVector3D DOWN_DIR(0, -1, 0);
        constexpr QVector3D LEFT_DIR(1, 0, 0);

        mTranslation += (mLinearSpeed * StepZ * ifps) * (GetRotation() * LEFT_DIR); // (...) are important
        mTranslation += (mLinearSpeed * StepW * ifps) * (GetRotation() * DOWN_DIR);
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
    Math::AddIfLess(mYaw, 0.0f, 360.0f);
    Math::AddIfGreater(mYaw, 360.0f, -360.0f);
    Math::AddIfLess(mPitch, 0.0f, 360.0f);
    Math::AddIfGreater(mPitch, 360.0f, -360.0f);
}

bool Canavar::Engine::PersecutorCamera::IsAnimating() const
{
    return mAnimator->IsAnimating();
}

bool Canavar::Engine::PersecutorCamera::ShouldIgnoreEvents() const
{
    return IsAnimating();
}
