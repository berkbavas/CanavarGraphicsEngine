#pragma once

#include "Canavar/Engine/Node/Object/LightningStrike/LightningStrikeAttractor.h"
#include "Canavar/Engine/Node/Object/LightningStrike/LightningStrikeBase.h"

#include <QSet>

namespace Canavar::Engine
{
    class LightningStrikeGenerator : public LightningStrikeBase
    {
        REGISTER_OBJECT_TYPE(LightningStrikeGenerator);

        void ToJson(QJsonObject& object) override;
        void FromJson(const QJsonObject& object, const std::map<QString, NodePtr>& nodes) override;

      public:
        LightningStrikeGenerator();

        QVector<QVector3D> GetWorldPositionsOfTerminationPoints() override;
        void AddAttractor(LightningStrikeAttractorPtr pAttractor);

      private:
        QSet<LightningStrikeAttractorPtr> mAttractors;
    };

    using LightningStrikeGeneratorPtr = std::shared_ptr<LightningStrikeGenerator>;
}