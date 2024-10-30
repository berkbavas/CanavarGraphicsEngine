#pragma once

#include "Canavar/Engine/Node/Object/Object.h"

namespace Canavar::Engine
{
    class DummyObject : public Object
    {
        REGISTER_OBJECT_TYPE(DummyObject);

      public:
        DummyObject();
    };

    using DummyObjectPtr = std::shared_ptr<DummyObject>;
}