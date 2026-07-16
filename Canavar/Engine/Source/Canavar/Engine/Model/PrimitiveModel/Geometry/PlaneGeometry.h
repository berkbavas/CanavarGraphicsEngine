#pragma once

#include "Canavar/Engine/Model/PrimitiveModel/Geometry/GeometryBase.h"

namespace Canavar::Engine
{
    // A unit quad in the XZ plane: covers [-0.5, 0.5] on X and Z, Y = 0.
    // Vertex layout: location 0 = position (vec3), location 1 = normal (vec3).
    // Use with the Primitive shader.
    class PlaneGeometry final : public GeometryBase
    {
      public:
        PlaneGeometry() = default;
        void Initialize() override;
    };

} // namespace Canavar::Engine
