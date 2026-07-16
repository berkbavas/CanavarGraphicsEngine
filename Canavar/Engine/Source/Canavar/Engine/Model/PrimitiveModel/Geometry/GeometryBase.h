#pragma once

#include <memory>

#include <QOpenGLExtraFunctions>

namespace Canavar::Engine
{
    class GeometryBase : protected QOpenGLExtraFunctions
    {
      public:
        GeometryBase() = default;
        virtual ~GeometryBase();

        GeometryBase(const GeometryBase &) = delete;
        GeometryBase &operator=(const GeometryBase &) = delete;

        virtual void Initialize() = 0;
        void Render();

      protected:
        GLuint mVAO{ 0 };
        GLuint mVBO{ 0 };
        GLuint mEBO{ 0 };
        GLsizei mCount{ 0 };
        GLenum mMode{ GL_TRIANGLES };
        bool mUseEBO{ false };
    };

    using GeometryBasePtr = std::unique_ptr<GeometryBase>;

} // namespace Canavar::Engine
