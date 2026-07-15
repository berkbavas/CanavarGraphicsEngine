#pragma once

#include "Canavar/Engine/Light/Light.h"

namespace Canavar::Engine
{
    class DirectionalLight : public Light
    {
      public:
        DirectionalLight();

        const char *GetNodeTypeName() const override { return "DirectionalLight"; }

        DEFINE_MEMBER(QVector3D, Direction, QVector3D(0, -1, 0));
        DEFINE_MEMBER(float, Radiance, 4.0f);
    };

    using DirectionalLightPtr = std::unique_ptr<DirectionalLight>;
}
