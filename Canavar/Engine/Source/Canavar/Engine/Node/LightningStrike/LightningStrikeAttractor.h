#pragma once

#include "Canavar/Engine/Node/Node.h"

#include <memory>

namespace Canavar::Engine
{
    class LightningStrikeAttractor : public Node
    {
      public:
        LightningStrikeAttractor();
    };

    using LightningStrikeAttractorPtr = std::shared_ptr<LightningStrikeAttractor>;
}