#pragma once

#include "Canavar/Engine/Node/Global/Global.h"
#include "Canavar/Engine/Node/Object/Light/DirectionalLight.h"

namespace Canavar::Engine
{
    class Sun : public DirectionalLight
    {
      public:
        Sun();

        const char *GetNodeTypeName() const override { return "Sun"; }
    };

    using SunPtr = std::shared_ptr<Sun>;
}
