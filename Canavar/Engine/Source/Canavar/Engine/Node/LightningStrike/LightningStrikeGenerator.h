#pragma once

#include "Canavar/Engine/Node/LightningStrike/LightningStrikeAttractor.h"
#include "Canavar/Engine/Node/LightningStrike/LightningStrikeBase.h"

#include <QSet>

namespace Canavar::Engine
{
    class LightningStrikeGenerator : public LightningStrikeBase
    {
      public:
        LightningStrikeGenerator();

        QVector<QVector3D> GetWorldPositionsOfTerminationPoints() override;
        void AddAttractor(LightningStrikeAttractorPtr Attractor);

      private:
        QSet<LightningStrikeAttractorPtr> mAttractors;
    };

    using LightningStrikeGeneratorPtr = std::shared_ptr<LightningStrikeGenerator>;
}