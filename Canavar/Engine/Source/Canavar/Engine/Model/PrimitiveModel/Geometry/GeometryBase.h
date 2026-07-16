#pragma once

#include <memory>

#include <QOpenGLExtraFunctions>

namespace Canavar::Engine
{
    class GeometryBase : protected QOpenGLExtraFunctions
    {
      public:
        GeometryBase() = default;
        virtual ~GeometryBase() = default;

        GeometryBase(const GeometryBase &) = delete;
        GeometryBase &operator=(const GeometryBase &) = delete;

        virtual void Render() = 0;
    };

    using GeometryBasePtr = std::unique_ptr<GeometryBase>;

}
