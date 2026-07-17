#pragma once

#include "Canavar/Engine/Object/Object.h"

namespace Canavar::Engine
{
    class DummyObject : public Object
    {
      public:
        DummyObject() = default;
        virtual ~DummyObject() = default;

        const char* GetNodeTypeName() const override { return "DummyObject"; }
    };

    using DummyObjectPtr = std::unique_ptr<DummyObject>;
}