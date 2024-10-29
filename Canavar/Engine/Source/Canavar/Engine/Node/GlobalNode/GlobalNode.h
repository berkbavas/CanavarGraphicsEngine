#pragma once

#include "Canavar/Engine/Node/Node.h"

namespace Canavar::Engine
{
    class GlobalNode : public Node
    {
      protected:
        GlobalNode() = default;

      public:
        virtual void Initialize();

        DEFINE_MEMBER(bool, Enabled, true);
    };

    using GlobalNodePtr = std::shared_ptr<GlobalNode>;
}