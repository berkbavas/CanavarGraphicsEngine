#pragma once

#include "Canavar/Engine/Node/Object/Object.h"

#include <memory>

namespace Canavar::Engine
{
    class LightningStrikeAttractor : public Object
    {
        REGISTER_NODE_TYPE(LightningStrikeAttractor);

      public:
        LightningStrikeAttractor();
    };

    using LightningStrikeAttractorPtr = std::shared_ptr<LightningStrikeAttractor>;
}