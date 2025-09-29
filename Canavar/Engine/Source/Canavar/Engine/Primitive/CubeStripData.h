#pragma once

#include "Canavar/Engine/Primitive/PrimitiveData.h"

#include <QOpenGLExtraFunctions>
#include <QVector3D>

namespace Canavar::Engine
{
    class CubeStripData : public PrimitiveData
    {
      public:
        CubeStripData();
        ~CubeStripData() override;

        void Render() override;

      private:
        GLuint mVAO{ 0 };
        GLuint mVBO{ 0 };
    };

    using CubeStripDataPtr = std::shared_ptr<CubeStripData>;
}
