#pragma once

#include "Canavar/Engine/Node/Object/Camera/FreeCameraAnimator.h"

namespace Canavar::Engine
{
    class FreeCameraSlerpAnimator : public FreeCameraAnimator
    {
      public:
        explicit FreeCameraSlerpAnimator(FreeCamera *pParent);
        ~FreeCameraSlerpAnimator() override = default;

        void Update(float ifps) override;
        bool IsAnimating() const override;
        void AnimateTo(const QVector3D &Position) override;
        void Stop() override;

      private:
        QVector3D mTargetPosition;
        QQuaternion mTargetRotation;
        QVector3D mInitialPosition;
        QQuaternion mInitialRotation;

        bool mAnimating{ false };

        float mTimeElapsed{ 0.0f };

        FreeCamera *mFreeCamera{ nullptr };

        static constexpr float DURATION{ 1.0f };    
        static constexpr float LINEAR_SPEED{ 4.0f };                          // Duration of the animation in seconds
        static constexpr QVector3D TRANSLATION_OFFSET{ 10.0f, 10.0f, -10.0f }; // Offset for the target position
    };

    using FreeCameraSlerpAnimatorPtr = std::shared_ptr<FreeCameraSlerpAnimator>;
}