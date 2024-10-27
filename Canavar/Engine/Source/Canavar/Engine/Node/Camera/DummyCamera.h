#pragma once

#include "Canavar/Engine/Node/Node.h"

namespace Canavar::Engine
{
    class DummyCamera : public Node
    {
      public:
        DummyCamera();
    };

    using DummyCameraPtr = std::shared_ptr<DummyCamera>;
}