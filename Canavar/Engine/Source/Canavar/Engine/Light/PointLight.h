#pragma once

#include "Canavar/Engine/Light/Light.h"

namespace Canavar::Engine
{
    class PointLight : public Light
    {
      public:
        PointLight();

        const char *GetNodeTypeName() const override { return "PointLight"; }

        DEFINE_MEMBER(float, Constant, 1.0f);
        DEFINE_MEMBER(float, Linear, 0.05f);
        DEFINE_MEMBER(float, Quadratic, 0.001f);
    };

    using PointLightPtr = std::unique_ptr<PointLight>;

}