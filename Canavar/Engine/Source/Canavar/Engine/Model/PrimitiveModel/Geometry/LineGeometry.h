#pragma once

#include "Canavar/Engine/Model/PrimitiveModel/Geometry/GeometryBase.h"

namespace Canavar::Engine
{
    // A line segment from (0,0,0) to (0,0,1) in local space.
    // Vertex layout: location 0 = position (vec3).
    // Use with the CircleLine shader (includes geometry stage for screen-space thickness).
    class LineGeometry final : public GeometryBase
    {
      public:
        LineGeometry();
        ~LineGeometry() override;

        void Render() override;

      private:
        GLuint mVAO{ 0 };
        GLuint mVBO{ 0 };
    };

} // namespace Canavar::Engine
