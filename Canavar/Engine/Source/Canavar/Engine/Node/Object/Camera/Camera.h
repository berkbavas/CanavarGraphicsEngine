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

        QVector2D Project3DTo2D(const QVector3D &pos3D);

        float CalculateSkyYOffset(float horizonDistance);

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
        QMatrix4x4 mTransformCacheForSkyYOffset;

        DEFINE_MEMBER(float, DevicePixelRatio, 1.0f);
    };

    using CameraPtr = std::shared_ptr<Camera>;
}