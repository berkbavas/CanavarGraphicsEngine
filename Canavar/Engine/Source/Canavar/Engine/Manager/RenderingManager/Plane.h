#pragma once

#include "Canavar/Engine/Node/Object/Object.h"

#include <QOpenGLExtraFunctions>

namespace Canavar::Engine
{
    class Plane : public Object, protected QOpenGLExtraFunctions
    {
        REGISTER_NODE_TYPE(Plane);

      public:
        Plane();

        void Render();

      private:
        GLuint mVAO{ 0 };
        GLuint mVBO{ 0 };
    };
}
