#pragma once

#include "Canavar/Engine/Node/Object/Light/DirectionalLight.h"

namespace Canavar::Engine
{
    class Sun : public DirectionalLight
    {
        REGISTER_NODE_TYPE(Sun);

      public:
        Sun();
    };

    using SunPtr = std::shared_ptr<Sun>;
}
