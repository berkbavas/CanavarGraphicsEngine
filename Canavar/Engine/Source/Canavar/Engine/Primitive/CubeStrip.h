#pragma once

#include <QOpenGLExtraFunctions>
#include <QVector3D>

namespace Canavar::Engine
{
    class CubeStrip : protected QOpenGLExtraFunctions
    {
      public:
        CubeStrip();

        void Render();

      private:
        GLuint mVAO{ 0 };
        GLuint mVBO{ 0 };
    };
}
