#pragma once

#include "Canavar/Engine/Object/Object.h"

namespace Canavar::Engine
{
    class Light : public Object
    {
      public:
        Light() = default;

        DEFINE_MEMBER(QVector3D, Color, QVector3D(1.0f, 1.0f, 1.0f));
        DEFINE_MEMBER(float, Ambient, 1.0f);
        DEFINE_MEMBER(float, Diffuse, 1.0f);
        DEFINE_MEMBER(float, Specular, 1.0f);
        DEFINE_MEMBER(bool, Enabled, true);
    };

    using LightPtr = std::unique_ptr<Light>;
}