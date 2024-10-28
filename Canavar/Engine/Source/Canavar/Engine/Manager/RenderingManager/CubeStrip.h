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

        static constexpr QVector3D CUBE_STRIP[] = {
            QVector3D(-0.5f, -0.5f, -0.5f), //
            QVector3D(-0.5f, -0.5f, 0.5f),  //
            QVector3D(-0.5f, 0.5f, 0.5f),   //
            QVector3D(-0.5f, 0.5f, -0.5f),  //
            QVector3D(-0.5f, -0.5f, -0.5f), //
            QVector3D(0.5f, -0.5f, -0.5f),  //
            QVector3D(0.5f, 0.5f, -0.5f),   //
            QVector3D(-0.5f, 0.5f, -0.5f),  //
            QVector3D(-0.5f, 0.5f, 0.5f),   //
            QVector3D(0.5f, 0.5f, 0.5f),    //
            QVector3D(0.5f, 0.5f, -0.5f),   //
            QVector3D(0.5f, -0.5f, -0.5f),  //
            QVector3D(0.5f, -0.5f, 0.5f),   //
            QVector3D(0.5f, 0.5f, 0.5f),    //
            QVector3D(-0.5f, 0.5f, 0.5f),   //
            QVector3D(-0.5f, -0.5f, 0.5f),  //
            QVector3D(0.5f, -0.5f, 0.5f),   //
        };
    };
}
