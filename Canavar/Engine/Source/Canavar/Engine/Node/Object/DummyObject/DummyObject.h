#pragma once

#include "Canavar/Engine/Node/Object/Object.h"

namespace Canavar::Engine
{
    class DummyObject : public Object
    {
      public:
        CANAVAR_NODE(DummyObject);
    };

    using DummyObjectPtr = std::shared_ptr<DummyObject>;
}