#pragma once

#include "Canavar/Engine/Primitive/PrimitiveData.h"

namespace Canavar::Engine
{
    class TorusData : public PrimitiveData
    {
      public:
        TorusData(float R, float r, int SegU, int SegV);
        ~TorusData() override;

        void Render() override;

      private:
        GLuint mVAO{ 0 };
        GLuint mVBO{ 0 };
        GLuint mEBO{ 0 };
        int mNumberOfIndices{ 0 };
    };

    using TorusDataPtr = std::shared_ptr<TorusData>;
}