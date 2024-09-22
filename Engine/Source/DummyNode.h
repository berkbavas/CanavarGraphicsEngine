#pragma once

#include "Node.h"

namespace Canavar {
    namespace Engine {
        class DummyNode : public Node
        {
        protected:
            friend class NodeManager;
            DummyNode();
        };
    } // namespace Engine
} // namespace Canavar
