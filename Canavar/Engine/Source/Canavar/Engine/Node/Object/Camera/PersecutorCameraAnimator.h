#pragma once

#include <functional>
#include <memory>

#include <QObject>

namespace Canavar::Engine
{
    class PersecutorCameraAnimator : public QObject
    {
        Q_OBJECT
      protected:
        explicit PersecutorCameraAnimator(QObject *pParent);
        virtual ~PersecutorCameraAnimator() = default;

      signals:
        void Updated(float Yaw, float Pitch);
        void AnimationFinished();

      public:
        virtual void Update(float ifps) = 0;
        virtual bool IsAnimating() const = 0;
        virtual void Animate(float CurrentYaw, float CurrentPitch, float TargetYaw, float TargetPitch) = 0;
        virtual void Stop() = 0;
    };

    using PersecutorCameraAnimatorPtr = std::shared_ptr<PersecutorCameraAnimator>;
}