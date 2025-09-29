#pragma once

#include "Canavar/Engine/Util/Macros.h"

#include <memory>

#include <QOpenGLExtraFunctions>

namespace Canavar::Engine
{
    class PrimitiveData : protected QOpenGLExtraFunctions
    {
        DISABLE_COPY(PrimitiveData);

      public:
        PrimitiveData() = default;
        virtual ~PrimitiveData() = default;

        virtual void Render() = 0;
    };

    using PrimitiveDataPtr = std::shared_ptr<PrimitiveData>;
}
