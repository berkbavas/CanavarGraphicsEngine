#pragma once

#include "Canavar/Engine/Node/Object/LightningStrike/LightningStrikeAttractor.h"
#include "Canavar/Engine/Node/Object/LightningStrike/LightningStrikeBase.h"

#include <QSet>

namespace Canavar::Engine
{
    class LightningStrikeGenerator : public LightningStrikeBase
    {
      public:
        CANAVAR_NODE(LightningStrikeGenerator);

        void ToJson(QJsonObject& object) override;
        void FromJson(const QJsonObject& object, const QSet<NodePtr>& nodes) override;

        QVector<QVector3D> GetWorldPositionsOfTerminationPoints() override;
        void AddAttractor(LightningStrikeAttractorPtr pAttractor);

      private:
        QSet<LightningStrikeAttractorPtr> mAttractors;
    };

    using LightningStrikeGeneratorPtr = std::shared_ptr<LightningStrikeGenerator>;
}