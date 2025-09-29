#pragma once

#include "Canavar/Engine/Node/Object/Camera/PersecutorCameraAnimator.h"

namespace Canavar::Engine
{
    class PersecutorCameraNonlinearAnimator : public PersecutorCameraAnimator
    {
        Q_OBJECT
      public:
        explicit PersecutorCameraNonlinearAnimator(QObject *pParent = nullptr);
        ~PersecutorCameraNonlinearAnimator() final = default;

        void Update(float ifps) override;
        bool IsAnimating() const override;
        void Animate(float CurrentYaw, float CurrentPitch, float TargetYaw, float TargetPitch) override;
        void Stop() override;
        void SetSpeed(float Speed);

      private:
        float ClampAngle(float Angle);

        float mCurrentYaw{ 0.0f };
        float mCurrentPitch{ 0.0f };

        float mTargetYaw{ 0.0f };
        float mTargetPitch{ 0.0f };

        float mSpeed{ 5.0f };

        bool mAnimating{ false };
    };

    using PersecutorCameraNonlinearAnimatorPtr = std::shared_ptr<PersecutorCameraNonlinearAnimator>;
}