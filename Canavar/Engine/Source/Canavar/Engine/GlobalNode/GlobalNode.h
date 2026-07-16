#pragma once

#include "Canavar/Engine/Node/Node.h"

namespace Canavar::Engine
{
    class Renderer;

    class GlobalNode : public Node
    {
      public:
        GlobalNode() = default;

    };

    using GlobalNodePtr = std::unique_ptr<GlobalNode>;
}