#pragma once

#include "Canavar/Engine/Primitive/PrimitiveData.h"

namespace Canavar::Engine
{
    class LineData : public PrimitiveData
    {
      public:
        LineData();
        ~LineData() override;

        void Render() override;

      private:
        GLuint mVAO{ 0 };
        GLuint mVBO{ 0 };

        static constexpr float DUMMY_POINT{ 0 };
    };

    using LineDataPtr = std::shared_ptr<LineData>;
}