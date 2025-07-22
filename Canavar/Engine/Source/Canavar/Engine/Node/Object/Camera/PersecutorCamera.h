#pragma once

#include "Canavar/Engine/Node/Object/Camera/PersecutorCameraNonlinearAnimator.h"
#include "Canavar/Engine/Node/Object/Camera/PerspectiveCamera.h"
#include "Canavar/Engine/Util/Mouse.h"

namespace Canavar::Engine
{
    class PersecutorCamera : public PerspectiveCamera, public QObject
    {
        REGISTER_NODE_TYPE(PersecutorCamera);

      public:
        PersecutorCamera();

        void MousePressed(QMouseEvent *) override;
        void MouseReleased(QMouseEvent *) override;
        void MouseMoved(QMouseEvent *) override;
        void WheelMoved(QWheelEvent *) override;
        void Update(float ifps) override;
        void Reset() override;

        void SetParent(ObjectWeakPtr pParentNode) override;
        void AddChild(ObjectPtr pNode) override;
        void RemoveChild(ObjectPtr pNode) override;

        ObjectPtr GetTarget() const;
        void SetTarget(ObjectPtr pNewTarget);

        void ToJson(QJsonObject &object) override;
        void FromJson(const QJsonObject &object, const std::map<QString, NodePtr> &nodes) override;

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
