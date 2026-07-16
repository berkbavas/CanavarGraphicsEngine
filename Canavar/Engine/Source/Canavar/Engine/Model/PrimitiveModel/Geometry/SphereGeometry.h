#pragma once

#include "Canavar/Engine/Model/PrimitiveModel/Geometry/GeometryBase.h"

#include <QVector3D>
#include <QVector>

namespace Canavar::Engine
{
    class SphereGeometry : public GeometryBase
    {
      public:
        explicit SphereGeometry(float Radius, int StackCount, int SectorCount);
        ~SphereGeometry() override;

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

        void CreateGeometry(float Radius, int StackCount, int SectorCount);

        GLuint mVAO{ 0 };
        GLuint mVBO{ 0 };
        GLuint mEBO{ 0 };

        QVector<Vertex> mVertices;
        QVector<TriangleFace> mFaces;

        int mNumberOfFaces{ 0 };
    };

    using SphereGeometryPtr = std::unique_ptr<SphereGeometry>;
}