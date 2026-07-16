#pragma once

#include "Canavar/Engine/Model/PrimitiveModel/Geometry/GeometryBase.h"

namespace Canavar::Engine
{
    // Generates a UV sphere of radius 1 centred at the origin.
    // Vertex layout: location 0 = position (vec3), location 1 = normal (vec3).
    // Use with the Primitive shader.
    class SphereGeometry final : public GeometryBase
    {
      public:
        explicit SphereGeometry(int Stacks = 32, int Sectors = 64);
        void Initialize() override;

      private:
        int mStacks;
        int mSectors;
    };

} // namespace Canavar::Engine
