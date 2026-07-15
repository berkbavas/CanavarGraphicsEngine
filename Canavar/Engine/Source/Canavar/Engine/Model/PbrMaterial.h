#pragma once

namespace Canavar::Engine
{
    class PbrMaterial
    {
      public:
        float Metallic{ 0.0f };
        float Roughness{ 0.0f };
        float AmbientOcclusion{ 0.003f };
    };
}