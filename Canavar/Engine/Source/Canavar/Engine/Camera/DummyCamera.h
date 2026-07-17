#pragma once

#include "Canavar/Engine/Camera/PerspectiveCamera.h"

namespace Canavar::Engine
{
    class DummyCamera : public PerspectiveCamera
    {
      public:
        DummyCamera() = default;
        virtual ~DummyCamera() = default;

        const char* GetNodeTypeName() const override { return "DummyCamera"; }
    };

    using DummyCameraPtr = std::unique_ptr<DummyCamera>;
}