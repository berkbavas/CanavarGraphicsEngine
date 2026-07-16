#pragma once

#include "Canavar/Engine/Model/PrimitiveModel/Geometry/GeometryBase.h"

namespace Canavar::Engine
{
    // Generates a unit circle in the XZ plane (Y=0), rendered as a line loop.
    // Vertex layout: location 0 = position (vec3).
    // Use with the CircleLine shader (includes geometry stage for screen-space thickness).
    class CircleGeometry final : public GeometryBase
    {
      public:
        explicit CircleGeometry(int Segments = 64);
        void Initialize() override;

      private:
        int mSegments;
    };

} // namespace Canavar::Engine
