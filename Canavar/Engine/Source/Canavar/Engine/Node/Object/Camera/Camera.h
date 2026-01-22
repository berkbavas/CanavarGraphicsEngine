#pragma once

#include "Canavar/Engine/Core/Constants.h"
#include "Canavar/Engine/Core/EventReceiver.h"
#include "Canavar/Engine/Node/Object/Object.h"

#include <QMouseEvent>

namespace Canavar::Engine
{
    class Camera : public Object, public EventReceiver
    {
      protected:
        Camera() = default;

      public:
        int GetWidth() const;
        int GetHeight() const;

        QVector2D ProjectWorldToScreenSpace(const QVector3D &WorldPosition) const;
        QVector3D UnprojectScreenToWorldSpace(const QVector2D &ScreenPos, float Depth) const;
        QVector3D ComputeRayFromScreenPosition(const QPointF &ScreenPos) const;

        virtual QMatrix4x4 GetProjectionMatrix() const = 0;
        virtual QMatrix4x4 GetViewProjectionMatrix() const = 0;
        virtual QMatrix4x4 GetRotationMatrix() const = 0;
        virtual QMatrix4x4 GetViewMatrix() const = 0;
        virtual QVector3D GetViewDirection() const = 0;

        virtual void Update(float ifps);
        virtual void Reset();
        virtual void Resize(int w, int h);

      protected:
        int mWidth{ INITIAL_WIDTH };
        int mHeight{ INITIAL_HEIGHT };

      private:
        DEFINE_MEMBER(float, DevicePixelRatio, 1.0f);
    };

    using CameraPtr = std::shared_ptr<Camera>;
}