#pragma once

#include "Canavar/Engine/Primitive/PrimitiveData.h"

namespace Canavar::Engine
{
    class SphereData : public PrimitiveData
    {
      public:
        SphereData(float Radius, int StackCount, int SectorCount);
        ~SphereData() override;

        void Render() override;

      private:
        GLuint mVAO{ 0 };
        GLuint mVBO{ 0 };
        GLuint mEBO{ 0 };

        int mNumberOfFaces{ 0 };
    };

    using SphereDataPtr = std::shared_ptr<SphereData>;
}