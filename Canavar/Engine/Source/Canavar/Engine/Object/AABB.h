#pragma once

#include <QMatrix4x4>
#include <QVector3D>

namespace Canavar::Engine
{
    class AABB
    {
      public:
        AABB() = default;
        AABB(const QVector3D& Min, const QVector3D& Max);

        const QVector3D& GetCenter() const;
        const QMatrix4x4& GetTransformation() const;
        const QVector3D& GetMin() const;
        const QVector3D& GetMax() const;

        AABB Transform(const QMatrix4x4& Transformation) const;

      private:
        QMatrix4x4 mTransformation;
        QVector3D mCenter;
        QVector3D mMin;
        QVector3D mMax;
    };

}