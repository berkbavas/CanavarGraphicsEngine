#pragma once

#include "Canavar/Engine/Node/Object/Object.h"

#include <memory>

namespace Canavar::Engine
{
    class PrimitiveMesh : public Object
    {
      public:
        PrimitiveMesh() = default;
    };

    class Plane : public PrimitiveMesh
    {
        CANAVAR_NODE(Plane);
    };

    class Quad : public PrimitiveMesh
    {
        CANAVAR_NODE(Quad);
    };

    class Cube : public PrimitiveMesh
    {
        CANAVAR_NODE(Cube);
    };

    class Sphere : public PrimitiveMesh
    {
        CANAVAR_NODE(Sphere);
    };

    class Circle : public PrimitiveMesh
    {
        CANAVAR_NODE(Circle);
    };

    class Line : public PrimitiveMesh
    {
        CANAVAR_NODE(Line);
    };

    class Torus : public PrimitiveMesh
    {
        CANAVAR_NODE(Torus);
    };

    using PrimitiveMeshPtr = std::shared_ptr<PrimitiveMesh>;
    using PlanePtr = std::shared_ptr<Plane>;
    using QuadPtr = std::shared_ptr<Quad>;
    using CubePtr = std::shared_ptr<Cube>;
    using SpherePtr = std::shared_ptr<Sphere>;
    using CirclePtr = std::shared_ptr<Circle>;
    using LinePtr = std::shared_ptr<Line>;
    using TorusPtr = std::shared_ptr<Torus>;

}