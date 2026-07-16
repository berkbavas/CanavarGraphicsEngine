#pragma once

#include "Canavar/Engine/Model/PrimitiveModel/PrimitiveModel.h"

namespace Canavar::Engine
{
    class PrimitiveModel;

    class OutlinePrimitive : public PrimitiveModel
    {
      public:
        explicit OutlinePrimitive(PrimitiveType Type);
        virtual ~OutlinePrimitive() = default;

        void Render(GeometryBase *pGeometry, Shader *pShader, PerspectiveCamera *pCamera) override;

        // Thickness of the outline in pixels; ignored for other primitive types
        DEFINE_MEMBER(float, Thickness, 2.0f);
    };

} // namespace Canavar::Engine