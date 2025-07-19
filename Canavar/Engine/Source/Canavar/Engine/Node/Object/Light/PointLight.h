#pragma once

#include "Canavar/Engine/Node/Object/Light/Light.h"

namespace Canavar::Engine
{
    class PointLight : public Light
    {
        REGISTER_NODE_TYPE(PointLight);

      public:
        PointLight();

        void ToJson(QJsonObject &object) override;
        void FromJson(const QJsonObject &object, const std::map<QString, NodePtr> &nodes) override;

        DEFINE_MEMBER(float, Constant, 1.0f);
        DEFINE_MEMBER(float, Linear, 0.05f);
        DEFINE_MEMBER(float, Quadratic, 0.001f);
    };

    using PointLightPtr = std::shared_ptr<PointLight>;

}