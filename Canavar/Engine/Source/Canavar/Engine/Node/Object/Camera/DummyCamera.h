#pragma once

#include "Canavar/Engine/Node/Object/Camera/PerspectiveCamera.h"

namespace Canavar::Engine
{
    class DummyCamera : public PerspectiveCamera
    {
      public:
        CANAVAR_NODE(DummyCamera);
    };

    using DummyCameraPtr = std::shared_ptr<DummyCamera>;
}