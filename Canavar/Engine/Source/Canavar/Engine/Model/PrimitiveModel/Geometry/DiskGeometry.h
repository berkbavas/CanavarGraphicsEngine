#pragma once

#include "Canavar/Engine/Model/PrimitiveModel/Geometry/GeometryBase.h"

namespace Canavar::Engine
{
    // Generates a filled unit disk in the XZ plane (Y=0).
    // Vertex layout: location 0 = position (vec3), location 1 = normal (vec3).
    // Use with the Primitive shader.
    class DiskGeometry final : public GeometryBase
    {
      public:
        explicit DiskGeometry(int Segments = 64);
        void Initialize() override;

      private:
        int mSegments;
    };

} // namespace Canavar::Engine
