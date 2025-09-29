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
        void Accept(NodeVisitor &Visitor) override;

        bool MousePressed(QMouseEvent *) override;
        bool MouseReleased(QMouseEvent *) override;
        bool MouseMoved(QMouseEvent *) override;
        bool WheelMoved(QWheelEvent *) override;
        void Update(float ifps) override;
        void Reset() override;

        ObjectPtr GetTarget() const;
        void SetTarget(ObjectPtr pNewTarget);

        void ToJson(QJsonObject &Object) override;
        void FromJson(const QJsonObject &Object, const QSet<NodePtr> &Nodes) override;

        void AnimateTo(float Yaw, float Pitch);
        void AnimateTo(ViewDirection ViewDirection);

        bool IsAnimating() const;

      private:
        void OnAnimationAnglesUpdated(float Yaw, float Pitch);
        void ClampAngles();
        bool ShouldIgnoreEvents() const;
        void HandleZoom(float ifps);
        void HandleRotation(float ifps);
        void HandleTranslation(float ifps);

        ObjectPtr mTarget;
        Mouse mMouse;

        DEFINE_MEMBER(float, Distance, 5.0f);
        DEFINE_MEMBER(float, Yaw, 0.0f);
        DEFINE_MEMBER(float, Pitch, 0.0f);
        DEFINE_MEMBER(float, AngularSpeed, 25.0f);
        DEFINE_MEMBER(float, AngularSpeedSmoothness, 0.25f);
        DEFINE_MEMBER(float, LinearSpeed, 1.0f);
        DEFINE_MEMBER(float, LinearSpeedSmoothness, 0.25f);
        DEFINE_MEMBER(float, ZoomStep, 0.5f);
        DEFINE_MEMBER(float, ZoomSmoothness, 10.0f);

        float mDistanceBuffer{ 0.0f };

        PersecutorCameraNonlinearAnimatorPtr mAnimator;

        QVector3D mTranslation;

        static constexpr float MIN_DISTANCE{ 0.1f };
        static constexpr float MAX_DISTANCE{ 20.0f };
    };

    using PersecutorCameraPtr = std::shared_ptr<PersecutorCamera>;
}
