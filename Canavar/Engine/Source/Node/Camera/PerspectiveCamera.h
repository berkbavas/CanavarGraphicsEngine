#pragma once

#include "Core/Constants.h"
#include "Node/Camera/Camera.h"
#include "Util/Macros.h"

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
        int GetWidth() const { return mWidth; }
        int GetHeight() const { return mHeight; }

        float GetHorizontalFov() const;
        float GetAspectRatio() const;

        void UpdateProjection();
        void UpdateCache();

      private:
        void UpdateProjectionIfDirty();
        void UpdateCacheIfDirty();

        // For caching
        QMatrix4x4 mViewProjectionMatrix;
        QMatrix4x4 mProjectionMatrix;
        QMatrix4x4 mViewMatrix;
        QMatrix4x4 mRotationMatrix;
        QVector3D mViewDirection;
        bool mIsProjectionDirty{ false };

        int mWidth{ INITIAL_WIDTH };
        int mHeight{ INITIAL_HEIGHT };

        DEFINE_MEMBER(float, ZNear, 0.1f);
        DEFINE_MEMBER(float, ZFar, 10000.0f);
        DEFINE_MEMBER(float, VerticalFov, 60.0f);
    };

}