#pragma once

#include "Canavar/Engine/Camera/PerspectiveCamera.h"
#include "Canavar/Engine/Core/Enums.h"

namespace Canavar::Engine
{
    struct RenderPassParameters
    {
        RenderPass RenderPass;
        PerspectiveCamera* pActiveCamera;
    };
}