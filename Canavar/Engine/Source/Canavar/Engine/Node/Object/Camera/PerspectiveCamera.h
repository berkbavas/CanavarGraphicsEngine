#pragma once

#include "Canavar/Engine/Core/Constants.h"
#include "Canavar/Engine/Node/Object/Camera/Camera.h"
#include "Canavar/Engine/Util/Macros.h"

namespace Canavar::Engine
{
    class PerspectiveCamera : public Camera
    {
      protected:
        PerspectiveCamera() = default;

      public:
        void Resize(int w, int h) override;

        const QMatrix4x4 &GetProjectionMatrix() override;
        const QMatrix4x4 &GetViewProjectionMatrix() override;
        const QMatrix4x4 &GetRotationMatrix() override;
        const QMatrix4x4 &GetViewMatrix() override;
        const QVector3D &GetViewDirection() override;
        int GetWidth() const override { return mWidth; }
        int GetHeight() const override { return mHeight; }

        float GetHorizontalFov() const;
        float GetAspectRatio() const;

        void ToJson(QJsonObject &object) override;
        void FromJson(const QJsonObject &object, const QSet<NodePtr> &nodes) override;

      private:
        // For caching
        QMatrix4x4 mViewProjectionMatrix;
        QMatrix4x4 mProjectionMatrix;
        QMatrix4x4 mViewMatrix;
        QMatrix4x4 mRotationMatrix;
        QVector3D mViewDirection;

        int mWidth{ INITIAL_WIDTH };
        int mHeight{ INITIAL_HEIGHT };

        DEFINE_MEMBER(float, ZNear, 0.1f);
        DEFINE_MEMBER(float, ZFar, 100'000.0f);
        DEFINE_MEMBER(float, VerticalFov, 60.0f);
    };

    using PerspectiveCameraPtr = std::shared_ptr<PerspectiveCamera>;
}
