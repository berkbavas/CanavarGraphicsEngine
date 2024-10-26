#pragma once

#include "Node/Node.h"

namespace Canavar::Engine
{
    class Light : public Node
    {
      protected:
        Light() = default;

        DEFINE_MEMBER(QVector4D, Color, QVector4D(1.0f, 1.0f, 1.0f, 1.0f));
        DEFINE_MEMBER(float, Ambient, 1.0f);
        DEFINE_MEMBER(float, Diffuse, 1.0f);
        DEFINE_MEMBER(float, Specular, 1.0f);
        DEFINE_MEMBER(bool, Enabled, true);
    };

    using LightPtr = std::shared_ptr<Light>;
}