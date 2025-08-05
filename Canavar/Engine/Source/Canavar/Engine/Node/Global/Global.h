#pragma once

#include "Canavar/Engine/Node/Node.h"

namespace Canavar::Engine
{
    class Global : public Node
    {
      protected:
        Global() = default;
    };

    using GlobalPtr = std::shared_ptr<Global>;
}