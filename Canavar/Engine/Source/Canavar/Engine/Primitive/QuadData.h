#pragma once

#include "Canavar/Engine/Primitive/PrimitiveData.h"

#include <QOpenGLExtraFunctions>

namespace Canavar::Engine
{
    class QuadData : public PrimitiveData
    {
      public:
        QuadData();
        ~QuadData() override;

        void Render() override;

      private:
        GLuint mVAO{ 0 };
        GLuint mVBO{ 0 };
    };

    using QuadDataPtr = std::shared_ptr<QuadData>;
}
