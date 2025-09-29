#pragma once

#include "Canavar/Engine/Primitive/PrimitiveData.h"

namespace Canavar::Engine
{
    class CircleData : public PrimitiveData
    {
      public:
        CircleData(int NumSegments = 96);
        ~CircleData() override;

        void Render() override;

      private:
        GLuint mVAO{ 0 };
        GLuint mVBO{ 0 };

        int mNumSegments{ 96 };
    };

    using CircleDataPtr = std::shared_ptr<CircleData>;
}
