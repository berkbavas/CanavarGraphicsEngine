#pragma once

#include <QOpenGLExtraFunctions>

namespace Canavar::Engine
{
    class Quad : protected QOpenGLExtraFunctions
    {
      public:
        Quad();

        void Render();

      private:
        GLuint mVAO{ 0 };
        GLuint mVBO{ 0 };
    };
}
