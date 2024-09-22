#pragma once

#include "Node.h"

#include <memory>

namespace Canavar::Engine
{
    class LightningStrikeAttractor : public Node
    {
    protected:
        friend class NodeManager;
        LightningStrikeAttractor();
    };

    using LightningStrikeAttractorWeakPtr = std::weak_ptr<LightningStrikeAttractor>;
    using LightningStrikeAttractorSharedPtr = std::shared_ptr<LightningStrikeAttractor>;
}