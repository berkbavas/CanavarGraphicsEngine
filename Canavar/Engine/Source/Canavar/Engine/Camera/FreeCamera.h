#pragma once

#include "Canavar/Engine/Camera/PerspectiveCamera.h"
#include "Canavar/Engine/Core/Mouse.h"

namespace Canavar::Engine
{
    class FreeCamera final : public PerspectiveCamera
    {
      public:
        FreeCamera();

        const char *GetNodeTypeName() const override { return "FreeCamera"; }

        void Update(float Ifps) override;
        void Reset() override;
        bool OnKeyPressed(QKeyEvent *) override;
        bool OnKeyReleased(QKeyEvent *) override;
        bool OnMousePressed(QMouseEvent *) override;
        bool OnMouseReleased(QMouseEvent *) override;
        bool OnMouseMoved(QMouseEvent *) override;
        bool OnLeaveEvent(QEvent *) override;

      private:
        bool IsKeyPressed(Qt::Key Key) const;
        float CalculateLinearSpeed(float Ifps) const;
        float CalculateAngularSpeed(float Ifps) const;

        // State
        float mYaw{ 0.0f };   // Rotation around the Y-axis
        float mPitch{ 0.0f }; // Rotation around the X-axis

        // Traits
        DEFINE_MEMBER(float, AngularSpeed, 15.0f);
        DEFINE_MEMBER(float, LinearSpeed, 5.0f);

        QMap<Qt::Key, bool> mPressedKeys; // Tracks the state of keys (pressed or not)
        Mouse mMouse;                     // Tracks the current state of the mouse (position, movement, buttons)

        static const QMap<Qt::Key, QVector3D> KEY_BINDINGS; // Maps keys to their corresponding movement directions in the camera's local space
    };

    using FreeCameraPtr = std::unique_ptr<FreeCamera>;
}