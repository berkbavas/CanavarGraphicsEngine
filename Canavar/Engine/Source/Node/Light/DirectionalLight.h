#pragma once

#include "Node/Light/Light.h"

namespace Canavar::Engine
{
    class DirectionalLight : public Light
    {
      public:
        DirectionalLight() = default;

        float GetTheta() const;
        float GetPhi() const;
        void SetDirectionFromThetaPhi(float theta, float phi);

        DEFINE_MEMBER(QVector3D, Direction, QVector3D(0, 1, 0));
    };

    using DirectionalLightPtr = std::shared_ptr<DirectionalLight>;
}
