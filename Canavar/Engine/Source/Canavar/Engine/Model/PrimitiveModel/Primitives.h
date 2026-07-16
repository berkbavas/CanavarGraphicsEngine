#pragma once

#include "Canavar/Engine/Model/PrimitiveModel/PrimitiveModel.h"

namespace Canavar::Engine
{
    class Circle final : public PrimitiveModel
    {
      public:
        Circle()
            : PrimitiveModel(PrimitiveType::Circle)
        {
            SetNodeName("Circle");
        }
    };

    class Disk final : public PrimitiveModel
    {
      public:
        Disk()
            : PrimitiveModel(PrimitiveType::Disk)
        {
            SetNodeName("Disk");
        }
    };

    class Line final : public PrimitiveModel
    {
      public:
        Line()
            : PrimitiveModel(PrimitiveType::Line)
        {
            SetNodeName("Line");
        }
    };

    class Plane final : public PrimitiveModel
    {
      public:
        Plane()
            : PrimitiveModel(PrimitiveType::Plane)
        {
            SetNodeName("Plane");
        }
    };

    class Sphere final : public PrimitiveModel
    {
      public:
        Sphere()
            : PrimitiveModel(PrimitiveType::Sphere)
        {
            SetNodeName("Sphere");
        }
    };

} // namespace Canavar::Engine
