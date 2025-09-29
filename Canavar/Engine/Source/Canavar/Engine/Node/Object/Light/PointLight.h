#pragma once

#include "Canavar/Engine/Node/Object/Light/Light.h"

namespace Canavar::Engine
{
    class PointLight : public Light
    {
      public:
        CANAVAR_NODE(PointLight);

        void ToJson(QJsonObject &Object) override;
        void FromJson(const QJsonObject &Object, const QSet<NodePtr> &Nodes) override;

        DEFINE_MEMBER(float, Constant, 1.0f);
        DEFINE_MEMBER(float, Linear, 0.05f);
        DEFINE_MEMBER(float, Quadratic, 0.001f);
    };

    using PointLightPtr = std::shared_ptr<PointLight>;

}