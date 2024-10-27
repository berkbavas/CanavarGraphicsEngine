#pragma once

#include "Canavar/Engine/Node/Light/Light.h"

namespace Canavar::Engine
{
    class PointLight : public Light
    {
      public:
        PointLight();

        DEFINE_MEMBER(float, Constant, 1.0f);
        DEFINE_MEMBER(float, Linear, 0.05f);
        DEFINE_MEMBER(float, Quadratic, 0.001f);
    };

    using PointLightPtr = std::shared_ptr<PointLight>;

}