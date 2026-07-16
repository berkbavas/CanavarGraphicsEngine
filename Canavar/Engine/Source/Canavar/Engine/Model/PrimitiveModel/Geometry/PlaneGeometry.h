#pragma once

#include "Canavar/Engine/Model/PrimitiveModel/Geometry/GeometryBase.h"

namespace Canavar::Engine
{
    class PlaneGeometry : public GeometryBase
    {
      public:
        PlaneGeometry();
        ~PlaneGeometry() override;

        void Render() override;

      private:
        GLuint mVAO{ 0 };
        GLuint mVBO{ 0 };
    };

    using PlaneGeometryPtr = std::unique_ptr<PlaneGeometry>;
}