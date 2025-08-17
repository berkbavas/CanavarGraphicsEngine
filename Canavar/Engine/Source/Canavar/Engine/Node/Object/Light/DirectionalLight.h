#pragma once

#include "Canavar/Engine/Node/Object/Light/Light.h"

namespace Canavar::Engine
{
    class DirectionalLight : public Light
    {
      public:
        CANAVAR_NODE(DirectionalLight);

        float GetTheta() const;
        float GetPhi() const;
        void SetDirectionFromThetaPhi(float theta, float phi);

        void ToJson(QJsonObject &object) override;
        void FromJson(const QJsonObject &object, const QSet<NodePtr> &nodes) override;

        DEFINE_MEMBER(QVector3D, Direction, QVector3D(0, 1, 0));
        DEFINE_MEMBER(float, Radiance, 5.0f);
    };

    using DirectionalLightPtr = std::shared_ptr<DirectionalLight>;
}
