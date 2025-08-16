#pragma once

#include "Canavar/Engine/Util/Macros.h"

#include <QMatrix4x4>
#include <QVector3D>

namespace Canavar::Engine
{
    class AABB
    {
      public:
        AABB() = default;
        AABB(const QVector3D& Min, const QVector3D& Max);

        const QVector3D& GetCenter() const { return mCenter; }
        const QMatrix4x4& GetTransformation() const { return mTransformation; }
        const QVector3D& GetMin() const { return mMin; }
        const QVector3D& GetMax() const { return mMax; }

        AABB Transform(const QMatrix4x4& Transformation) const;

      private:
        QMatrix4x4 mTransformation;
        QVector3D mCenter;
        QVector3D mMin;
        QVector3D mMax;
    };
}
