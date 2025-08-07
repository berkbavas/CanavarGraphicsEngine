#pragma once

#include <memory>

#include <QObject>
#include <QQuaternion>

namespace Canavar::Engine
{
    class FreeCamera;

    // Base class for animators that control the FreeCamera's position and rotation
    // This class is intended to be subclassed for specific animation behaviors
    // It provides a signal for updates and animation completion
    class FreeCameraAnimator : public QObject
    {
        Q_OBJECT
      protected:
        explicit FreeCameraAnimator(FreeCamera *pParent);
        virtual ~FreeCameraAnimator() = default;

      signals:
        void AnimationFinished();

      public:
        virtual void Update(float ifps) = 0;
        virtual bool IsAnimating() const = 0;
        virtual void AnimateTo(const QVector3D &Position) = 0;
        virtual void Stop() = 0;
    };

    using FreeCameraAnimatorPtr = std::shared_ptr<FreeCameraAnimator>;
}