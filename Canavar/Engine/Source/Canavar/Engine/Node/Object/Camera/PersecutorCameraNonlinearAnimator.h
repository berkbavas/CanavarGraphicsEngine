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
        void Animate(float currentYaw, float currentPitch, float targetYaw, float targetPitch) override;
        void Stop() override;
        void SetSpeed(float speed);

      private:
        float ClampAngle(float angle);

        float mCurrentYaw{ 0.0f };
        float mCurrentPitch{ 0.0f };

        float mTargetYaw{ 0.0f };
        float mTargetPitch{ 0.0f };

        float mSpeed{ 5.0f };

        bool mAnimating{ false };
    };

    using PersecutorCameraNonlinearAnimatorPtr = std::shared_ptr<PersecutorCameraNonlinearAnimator>;
}