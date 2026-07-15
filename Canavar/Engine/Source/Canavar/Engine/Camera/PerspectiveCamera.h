#pragma once

#include "Canavar/Engine/Camera/Camera.h"

namespace Canavar::Engine
{
    class PerspectiveCamera : public Camera
    {
      public:
        PerspectiveCamera() = default;

        QMatrix4x4 GetProjectionMatrix() const override;

        float GetHorizontalFov() const;
        float GetAspectRatio() const;

        DEFINE_MEMBER(float, ZNear, 0.1f);        // Near clipping plane distance
        DEFINE_MEMBER(float, ZFar, 100'000.0f);   // Far clipping plane distance
        DEFINE_MEMBER(float, VerticalFov, 60.0f); // Vertical field of view in degree
    };

    using PerspectiveCameraPtr = std::unique_ptr<PerspectiveCamera>;
}