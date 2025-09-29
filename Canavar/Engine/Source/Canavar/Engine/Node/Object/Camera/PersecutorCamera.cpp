#include "PersecutorCamera.h"

#include "Canavar/Engine/Node/NodeVisitor.h"
#include "Canavar/Engine/Util/Logger.h"
#include "Canavar/Engine/Util/Math.h"

Canavar::Engine::PersecutorCamera::PersecutorCamera()
{
    SetNodeName("Persecutor Camera");

    mAnimator = std::make_shared<PersecutorCameraNonlinearAnimator>();

    connect(mAnimator.get(), &PersecutorCameraAnimator::Updated, this, &PersecutorCamera::OnAnimationAnglesUpdated);
}

void Canavar::Engine::PersecutorCamera::Accept(NodeVisitor& Visitor)
{
    Visitor.Visit(*this);
}

bool Canavar::Engine::PersecutorCamera::MousePressed(QMouseEvent* pEvent)
{
    if (ShouldIgnoreEvents())
    {
        return false;
    }

    mMouse.X = pEvent->position().x();
    mMouse.Y = pEvent->position().y();
    mMouse.Z = pEvent->position().x();
    mMouse.W = pEvent->position().y();
    mMouse.Button = pEvent->button();

    return true;
}

bool Canavar::Engine::PersecutorCamera::MouseReleased(QMouseEvent* pEvent)
{
    if (ShouldIgnoreEvents())
    {
        return false;
    }

    if (mMouse.Button == pEvent->button())
    {
        mMouse.Button = Qt::NoButton;
    }

    return false;
}

bool Canavar::Engine::PersecutorCamera::MouseMoved(QMouseEvent* pEvent)
{
    if (ShouldIgnoreEvents())
    {
        return false;
    }

    float x = pEvent->position().x();
    float y = pEvent->position().y();

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

bool Canavar::Engine::PersecutorCamera::WheelMoved(QWheelEvent* pEvent)
{
    if (ShouldIgnoreEvents())
    {
        return false;
    }

    if (pEvent->angleDelta().y() < 0)
    {
        mDistanceBuffer += mZoomStep;
    }

    if (pEvent->angleDelta().y() > 0)
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
    LOG_DEBUG("PersecutorCamera::SetTarget: mTarget: {}, pNewTarget: {}", static_cast<void*>(mTarget.get()), static_cast<void*>(pNewTarget.get()));

    if (pNewTarget == nullptr)
    {
        LOG_INFO("PersecutorCamera::SetTarget: pNewTarget is nullptr.");
    }

    mTarget = pNewTarget;
    Reset();
}

void Canavar::Engine::PersecutorCamera::ToJson(QJsonObject& Object)
{
    PerspectiveCamera::ToJson(Object);

    if (mTarget)
    {
        Object.insert("target_uuid", QString::fromStdString(mTarget->GetUuid()));
    }

    Object.insert("angular_speed", mAngularSpeed);
}

void Canavar::Engine::PersecutorCamera::FromJson(const QJsonObject& Object, const QSet<NodePtr>& Nodes)
{
    PerspectiveCamera::FromJson(Object, Nodes);

    mAngularSpeed = Object["angular_speed"].toDouble(25.0f);

    QString TargetUuid = Object["target_uuid"].toString("");

    if (TargetUuid.isEmpty() == false)
    {
        LOG_DEBUG("PersecutorCamera::FromJson: UUID of my target is {}.", TargetUuid.toStdString());

        for (const auto pNode : Nodes)
        {
            if (TargetUuid == pNode->GetUuid())
            {
                LOG_DEBUG("PersecutorCamera::FromJson: I have found my target.");
                SetTarget(std::dynamic_pointer_cast<Canavar::Engine::Object>(pNode));
            }
        }
    }
    else
    {
        LOG_DEBUG("PersecutorCamera::FromJson: I have no targets.");
    }
}

void Canavar::Engine::PersecutorCamera::AnimateTo(float Yaw, float Pitch)
{
    mMouse.Reset();
    mAnimator->Animate(mYaw, mPitch, Yaw, Pitch);
}

void Canavar::Engine::PersecutorCamera::AnimateTo(ViewDirection ViewDirection)
{
    switch (ViewDirection)
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

void Canavar::Engine::PersecutorCamera::OnAnimationAnglesUpdated(float Yaw, float Pitch)
{
    mYaw = Yaw;
    mPitch = Pitch;
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
