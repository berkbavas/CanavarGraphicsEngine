#pragma once

#include "Canavar/Engine/Core/Constants.h"
#include "Canavar/Engine/Node/Object/Camera/Camera.h"
#include "Canavar/Engine/Util/Macros.h"
#include "Canavar/Engine/Util/Mouse.h"

namespace Canavar::Engine
{
    class OrthographicCamera : public Camera
    {
        REGISTER_NODE_TYPE(OrthographicCamera);

      public:
        OrthographicCamera() = default;

        void Resize(int width, int height) override;

        void MousePressed(QMouseEvent* event) override;
        void MouseReleased(QMouseEvent* event) override;
        void MouseMoved(QMouseEvent* event) override;
        void WheelMoved(QWheelEvent* event) override;

        const QMatrix4x4& GetProjectionMatrix();
        const QMatrix4x4& GetViewProjectionMatrix() override;
        const QMatrix4x4& GetRotationMatrix() override;
        const QMatrix4x4& GetViewMatrix() override;
        const QVector3D& GetViewDirection() override;

        int GetWidth() const override { return mWidth; }
        int GetHeight() const override { return mHeight; }

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
        // For caching
        QMatrix4x4 mViewProjectionMatrix;
        QMatrix4x4 mProjectionMatrix;
        QMatrix4x4 mViewMatrix;
        QMatrix4x4 mRotationMatrix;
        QVector3D mViewDirection;

        int mWidth{ INITIAL_WIDTH };
        int mHeight{ INITIAL_HEIGHT };

        DEFINE_MEMBER(float, Zoom, 1.0f);
        DEFINE_MEMBER(float, Top, 0.0f);
        DEFINE_MEMBER(float, Left, 0.0f);
        DEFINE_MEMBER(float, ZNear, -1.0f);
        DEFINE_MEMBER(float, ZFar, 1.0f);
        DEFINE_MEMBER(Qt::MouseButton, ActionReceiveButton, Qt::MiddleButton)

        Mouse mMouse;
    };
}
