#pragma once

#include "Canavar/Engine/Node/Node.h"

namespace Canavar::Engine
{
    class DummyNode : public Node
    {
      public:
        DummyNode();
    };

    using DummyNodePtr = std::shared_ptr<Node>;
}