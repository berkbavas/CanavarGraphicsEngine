#pragma once

#include "Canavar/Engine/Core/Structs.h"
#include "Canavar/Engine/Node/Object/Camera/PersecutorCameraNonlinearAnimator.h"
#include "Canavar/Engine/Node/Object/Camera/PerspectiveCamera.h"

namespace Canavar::Engine
{
    class PersecutorCamera : public PerspectiveCamera, public QObject
    {
      public:
        PersecutorCamera();

        const char *GetNodeTypeName() const override { return "PersecutorCamera"; }

        void MousePressed(QMouseEvent *) override;
        void MouseReleased(QMouseEvent *) override;
        void MouseMoved(QMouseEvent *) override;
        void WheelMoved(QWheelEvent *) override;
        void Update(float ifps) override;
        void Reset() override;

        ObjectPtr GetTarget() const;
        void SetTarget(ObjectPtr pNewTarget);

        void ToJson(QJsonObject &object) override;
        void FromJson(const QJsonObject &object, const QSet<NodePtr> &nodes) override;

        void AnimateTo(float yaw, float pitch);
        void AnimateTo(ViewDirection viewDirection);

        bool IsAnimating() const;

      private:
        void OnAnimationAnglesUpdated(float yaw, float pitch);
        void ClampAngles();
        bool ShouldIgnoreEvents() const;

        ObjectPtr mTarget;
        Mouse mMouse;

        DEFINE_MEMBER(float, AngularSpeed, 25.0f);
        DEFINE_MEMBER(float, AngularSpeedMultiplier, 1.0f);
        DEFINE_MEMBER(float, LinearSpeed, 1.0f);
        DEFINE_MEMBER(float, Distance, 5.0f);
        DEFINE_MEMBER(float, Yaw, 0.0f);
        DEFINE_MEMBER(float, Pitch, 0.0f);

        PersecutorCameraNonlinearAnimatorPtr mAnimator;

        QVector3D mTranslation;
    };

    using PersecutorCameraPtr = std::shared_ptr<PersecutorCamera>;
}
