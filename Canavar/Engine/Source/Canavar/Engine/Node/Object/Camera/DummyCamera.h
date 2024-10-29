#pragma once

#include "Canavar/Engine/Node/Object/Camera/PerspectiveCamera.h"

namespace Canavar::Engine
{
    class DummyCamera : public PerspectiveCamera
    {
      public:
        DummyCamera();
    };

    using DummyCameraPtr = std::shared_ptr<DummyCamera>;
}