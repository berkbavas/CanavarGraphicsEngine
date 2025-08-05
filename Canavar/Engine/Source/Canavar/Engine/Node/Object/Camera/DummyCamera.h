#pragma once

#include "Canavar/Engine/Node/Object/Camera/PerspectiveCamera.h"

namespace Canavar::Engine
{
    class DummyCamera : public PerspectiveCamera
    {
      public:
        DummyCamera();

        const char *GetNodeTypeName() const override { return "DummyCamera"; }
    };

    using DummyCameraPtr = std::shared_ptr<DummyCamera>;
}