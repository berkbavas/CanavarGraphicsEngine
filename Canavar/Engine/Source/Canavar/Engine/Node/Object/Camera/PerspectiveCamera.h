#pragma once

#include "Canavar/Engine/Node/Object/Camera/Camera.h"
#include "Canavar/Engine/Util/Macros.h"

namespace Canavar::Engine
{
    class PerspectiveCamera : public Camera
    {
      protected:
        PerspectiveCamera() = default;

      public:
        QMatrix4x4 GetProjectionMatrix() const override;
        QMatrix4x4 GetViewProjectionMatrix() const override;
        QMatrix4x4 GetRotationMatrix() const override;
        QMatrix4x4 GetViewMatrix() const override;
        QVector3D GetViewDirection() const override;
      
        float GetHorizontalFov() const;
        float GetAspectRatio() const;

        void ToJson(QJsonObject &object) override;
        void FromJson(const QJsonObject &object, const QSet<NodePtr> &nodes) override;

      private:
        DEFINE_MEMBER(float, ZNear, 0.1f);
        DEFINE_MEMBER(float, ZFar, 100'000.0f);
        DEFINE_MEMBER(float, VerticalFov, 60.0f);
    };

    using PerspectiveCameraPtr = std::shared_ptr<PerspectiveCamera>;
}
