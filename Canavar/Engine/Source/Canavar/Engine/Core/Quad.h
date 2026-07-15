#pragma once

#include <memory>

#include <QOpenGLFunctions_4_5_Core>

namespace Canavar::Engine
{
    class Quad : protected QOpenGLFunctions_4_5_Core
    {
      public:
        Quad();
        Quad(const Quad&) = delete;
        Quad& operator=(const Quad&) = delete;
        ~Quad();

        void Render();

      private:
        GLuint mVAO{ 0 };
        GLuint mVBO{ 0 };
    };

    using QuadPtr = std::unique_ptr<Quad>;

}