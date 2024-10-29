#pragma once

#include "Canavar/Engine/Node/Object/Object.h"

#include <memory>

namespace Canavar::Engine
{
    class LightningStrikeAttractor : public Object
    {
      public:
        LightningStrikeAttractor();
    };

    using LightningStrikeAttractorPtr = std::shared_ptr<LightningStrikeAttractor>;
}