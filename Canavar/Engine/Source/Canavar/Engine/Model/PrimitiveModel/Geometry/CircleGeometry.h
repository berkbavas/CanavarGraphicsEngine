#pragma once

#include "Canavar/Engine/Model/PrimitiveModel/Geometry/GeometryBase.h"

#include <QVector3D>
#include <QVector>

namespace Canavar::Engine
{
    class CircleGeometry : public GeometryBase
    {
      public:
        explicit CircleGeometry(int NumSegments = 96);
        ~CircleGeometry() override;

        void Render() override;

      private:
        static QVector<QVector3D> CreateGeometry(int NumSegments);

        int mNumSegments{ 96 };

        GLuint mVAO{ 0 };
        GLuint mVBO{ 0 };
    };

    using CircleGeometryPtr = std::unique_ptr<CircleGeometry>;
}