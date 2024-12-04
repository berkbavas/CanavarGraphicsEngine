#pragma once

#include "Canavar/Engine/Node/Object/Object.h"

#include <memory>

namespace Canavar::Engine
{
    class LightningStrikeAttractor : public Object
    {
        REGISTER_OBJECT_TYPE(LightningStrikeAttractor);

        void ToJson(QJsonObject &object) override;
        void FromJson(const QJsonObject &object, const std::map<QString, NodePtr> &nodes) override;

      public:
        LightningStrikeAttractor();
    };

    using LightningStrikeAttractorPtr = std::shared_ptr<LightningStrikeAttractor>;
}