#pragma once

namespace Canavar::Engine
{
    struct PhongMaterial
    {
        float Ambient{ 0.2f };
        float Diffuse{ 0.7f };
        float Specular{ 0.1f };
        float Shininess{ 32.0f };
    };
}