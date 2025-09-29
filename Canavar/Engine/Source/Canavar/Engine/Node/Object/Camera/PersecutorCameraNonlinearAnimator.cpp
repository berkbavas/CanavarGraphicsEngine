#include "PersecutorCameraNonlinearAnimator.h"

Canavar::Engine::PersecutorCameraNonlinearAnimator::PersecutorCameraNonlinearAnimator(QObject *pParent)
    : PersecutorCameraAnimator(pParent)
{}

void Canavar::Engine::PersecutorCameraNonlinearAnimator::Update(float ifps)
{
    if (!mAnimating)
    {
        return;
    }

    float deltaYaw = ClampAngle(mTargetYaw - mCurrentYaw);
    float deltaPitch = ClampAngle(mTargetPitch - mCurrentPitch);

    mCurrentYaw += deltaYaw * mSpeed * ifps;
    mCurrentPitch += deltaPitch * mSpeed * ifps;

    emit Updated(mCurrentYaw, mCurrentPitch);

    if (std::abs(deltaYaw) < 0.5f && std::abs(deltaPitch) < 0.5f)
    {
        mAnimating = false;
        mCurrentYaw = mTargetYaw;
        mCurrentPitch = mTargetPitch;
        emit Updated(mCurrentYaw, mCurrentPitch);
        emit AnimationFinished();
    }
}

bool Canavar::Engine::PersecutorCameraNonlinearAnimator::IsAnimating() const
{
    return mAnimating;
}

void Canavar::Engine::PersecutorCameraNonlinearAnimator::Animate(float CurrentYaw, float CurrentPitch, float TargetYaw, float TargetPitch)
{
    mCurrentYaw = CurrentYaw;
    mCurrentPitch = CurrentPitch;
    mTargetYaw = TargetYaw;
    mTargetPitch = TargetPitch;
    mAnimating = true;
}

void Canavar::Engine::PersecutorCameraNonlinearAnimator::Stop()
{
    mAnimating = false;
}

void Canavar::Engine::PersecutorCameraNonlinearAnimator::SetSpeed(float Speed)
{
    mSpeed = Speed;
}

float Canavar::Engine::PersecutorCameraNonlinearAnimator::ClampAngle(float Angle)
{
    if (Angle < -180)
    {
        Angle += 360;
    }

    if (Angle > 180)
    {
        Angle -= 360;
    }

    return Angle;
}
