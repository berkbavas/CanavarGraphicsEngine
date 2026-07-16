#pragma once

#include "Canavar/Engine/Model/PrimitiveModel/OutlinePrimitive.h"
#include "Canavar/Engine/Model/PrimitiveModel/PrimitiveModel.h"

namespace Canavar::Engine
{
    class Line final : public OutlinePrimitive
    {
      public:
        Line();

        void SetPoints(const QVector3D &StartPoint, const QVector3D &EndPoint);
    };

    class Circle final : public OutlinePrimitive
    {
      public:
        Circle();
    };

    class Disk final : public PrimitiveModel
    {
      public:
        Disk();
    };

    class Plane final : public PrimitiveModel
    {
      public:
        Plane();
    };

    class Sphere final : public PrimitiveModel
    {
      public:
        Sphere();
    };

} // namespace Canavar::Engine
