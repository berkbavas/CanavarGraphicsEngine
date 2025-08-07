#include "FreeCameraSlerpAnimator.h"

#include "Canavar/Engine/Node/Object/Camera/FreeCamera.h"
#include "Canavar/Engine/Util/Math.h"

#include <QMatrix4x4>

Canavar::Engine::FreeCameraSlerpAnimator::FreeCameraSlerpAnimator(FreeCamera *pParent)
    : FreeCameraAnimator(pParent)
    , mFreeCamera(pParent)
{}

void Canavar::Engine::FreeCameraSlerpAnimator::Update(float ifps)
{
    if (!mAnimating)
    {
        return;
    }

    mTimeElapsed += ifps;

    const auto CurrentPosition = mFreeCamera->GetWorldPosition();
    const auto PositionToGo = mTargetPosition - CurrentPosition;
    const auto DistanceToTarget = PositionToGo.length();

    if (PositionToGo.lengthSquared() < 0.1f)
    {
        Stop();
    }

    float t = 1 - DistanceToTarget / (mTargetPosition - mInitialPosition).length();

    const auto NewPosition = CurrentPosition + PositionToGo * LINEAR_SPEED * ifps;
    const auto CurrentRotation = QQuaternion::slerp(mInitialRotation, mTargetRotation, t);

    mFreeCamera->SetWorldPosition(NewPosition);
    mFreeCamera->SetWorldRotation(CurrentRotation);
}

bool Canavar::Engine::FreeCameraSlerpAnimator::IsAnimating() const
{
    return mAnimating;
}

void Canavar::Engine::FreeCameraSlerpAnimator::AnimateTo(const QVector3D &Position)
{
    mTargetPosition = Position + TRANSLATION_OFFSET;

    const auto ActualTargetRotation = Math::RotationBetweenVectors(QVector3D(0, 0, 1), TRANSLATION_OFFSET.normalized());
    const auto EulerAngles = ActualTargetRotation.toEulerAngles();

    mTargetRotation = QQuaternion::fromEulerAngles(EulerAngles.x(), EulerAngles.y(), 0);
    mInitialPosition = mFreeCamera->GetWorldPosition();
    mInitialRotation = mFreeCamera->GetWorldRotation();
    mAnimating = true;
}

void Canavar::Engine::FreeCameraSlerpAnimator::Stop()
{
    mAnimating = false;
    emit AnimationFinished();
}
