#pragma once

#include "Canavar/Engine/Camera/PerspectiveCamera.h"
#include "Canavar/Engine/Core/Mouse.h"

namespace Canavar::Engine
{
    class PersecutorCamera final : public PerspectiveCamera
    {
      public:
        PersecutorCamera();

        const char *GetNodeTypeName() const override { return "PersecutorCamera"; }

        bool OnMousePressed(QMouseEvent *) override;
        bool OnMouseReleased(QMouseEvent *) override;
        bool OnMouseMoved(QMouseEvent *) override;
        bool OnWheelMoved(QWheelEvent *) override;
        void Update(float Ifps) override;
        void Reset() override;

        Object *GetTarget() const;
        void SetTarget(Object *pNewTarget);

      private:
        void ClampAngles();
        bool ShouldIgnoreEvents() const;
        void HandleZoom(float ifps);
        void HandleRotation(float ifps);
        void HandleTranslation(float ifps);

        // Internal state
        Object *mTarget{ nullptr };
        Mouse mMouse;
        float mDistanceBuffer{ 0.0f };
        QVector3D mTranslation;

        float mYaw{ 0.0f };
        float mPitch{ 0.0f };
        float mDistance{ 5.0f };

        // Traits
        DEFINE_MEMBER(float, AngularSpeed, 25.0f);
        DEFINE_MEMBER(float, AngularSpeedSmoothness, 0.25f);
        DEFINE_MEMBER(float, LinearSpeed, 2.5f);
        DEFINE_MEMBER(float, LinearSpeedSmoothness, 0.25f);
        DEFINE_MEMBER(float, ZoomStep, 0.5f);
        DEFINE_MEMBER(float, ZoomSmoothness, 10.0f);

        static constexpr float MIN_DISTANCE{ 0.1f };
        static constexpr float MAX_DISTANCE{ 100.0f };
    };

    using PersecutorCameraPtr = std::unique_ptr<PersecutorCamera>;
}
