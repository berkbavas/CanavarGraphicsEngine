#pragma once

#include "Canavar/Engine/Node/Object/Object.h"

#include <memory>

namespace Canavar::Engine
{
    class LightningStrikeAttractor : public Object
    {
      public:
        LightningStrikeAttractor();

        const char* GetNodeTypeName() const override { return "LightningStrikeAttractor"; }
    };

    using LightningStrikeAttractorPtr = std::shared_ptr<LightningStrikeAttractor>;
} 