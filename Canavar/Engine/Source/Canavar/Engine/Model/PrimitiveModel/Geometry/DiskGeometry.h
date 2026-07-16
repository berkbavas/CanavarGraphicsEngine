#pragma once

#include "Canavar/Engine/Model/PrimitiveModel/Geometry/GeometryBase.h"

#include <QVector3D>
#include <QVector>

namespace Canavar::Engine
{
    class DiskGeometry : public GeometryBase
    {
      public:
        explicit DiskGeometry(float Radius, int SegmentCount);
        ~DiskGeometry() override;

        void Render() override;

      private:
        struct Vertex
        {
            QVector3D Position;
            QVector3D Normal;
        };

        struct TriangleFace
        {
            unsigned int Index0;
            unsigned int Index1;
            unsigned int Index2;
        };

        void CreateGeometry(float Radius, int SegmentCount);

        GLuint mVAO{ 0 };
        GLuint mVBO{ 0 };
        GLuint mEBO{ 0 };

        QVector<Vertex> mVertices;
        QVector<TriangleFace> mFaces;

        int mNumberOfFaces{ 0 };
    };

    using DiskGeometryPtr = std::unique_ptr<DiskGeometry>;
}