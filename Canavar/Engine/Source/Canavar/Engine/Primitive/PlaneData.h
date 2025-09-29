#pragma once

#include "Canavar/Engine/Primitive/PrimitiveData.h"

#include <QOpenGLExtraFunctions>

namespace Canavar::Engine
{
    class PlaneData : public PrimitiveData
    {
      public:
        PlaneData();
        ~PlaneData() override;
        void Render() override;

      private:
        GLuint mVAO{ 0 };
        GLuint mVBO{ 0 };
    };

    using PlaneDataPtr = std::shared_ptr<PlaneData>;
}
