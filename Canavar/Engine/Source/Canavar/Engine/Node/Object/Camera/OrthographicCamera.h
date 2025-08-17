#pragma once

#include "Canavar/Engine/Core/Constants.h"
#include "Canavar/Engine/Core/Structs.h"
#include "Canavar/Engine/Node/Object/Camera/Camera.h"
#include "Canavar/Engine/Util/Macros.h"

namespace Canavar::Engine
{
    class OrthographicCamera : public Camera
    {
      public:
        CANAVAR_NODE(OrthographicCamera);

        bool MousePressed(QMouseEvent* event) override;
        bool MouseReleased(QMouseEvent* event) override;
        bool MouseMoved(QMouseEvent* event) override;
        bool WheelMoved(QWheelEvent* event) override;

        QMatrix4x4 GetProjectionMatrix() const override;
        QMatrix4x4 GetViewProjectionMatrix() const override;
        QMatrix4x4 GetRotationMatrix() const override;
        QMatrix4x4 GetViewMatrix() const override;
        QVector3D GetViewDirection() const override;

        float GetAspectRatio() const;

        QVector2D CameraToWorld(float x, float y);
        QVector2D CameraToWorld(const QVector2D& camera);
        QPointF CameraToWorld(const QPointF& camera);

        QVector2D WorldToCamera(float x, float y);
        QVector2D WorldToCamera(const QVector2D& world);
        QPointF WorldToCamera(const QPointF& world);

        float WorldDistanceToCameraDistance(float distance);
        float CameraDistanceToWorldDistance(float distance);

        void Reset();

      private:
        DEFINE_MEMBER(float, Zoom, 1.0f);
        DEFINE_MEMBER(float, Top, 0.0f);
        DEFINE_MEMBER(float, Left, 0.0f);
        DEFINE_MEMBER(float, ZNear, -1.0f);
        DEFINE_MEMBER(float, ZFar, 1.0f);
        DEFINE_MEMBER(Qt::MouseButton, ActionReceiveButton, Qt::MiddleButton)

        Mouse mMouse;
    };
}
