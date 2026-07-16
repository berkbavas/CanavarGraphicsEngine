#pragma once

#include "Canavar/Engine/Core/Enums.h"
#include "Canavar/Engine/Object/Object.h"
#include "Canavar/Engine/Util/Macros.h"

#include <memory>

#include <QVector3D>

namespace Canavar::Engine
{
    // Base class for all procedurally-generated primitive models.
    // The PrimitiveType enum passed to the constructor identifies which geometry
    // will be fetched from PrimitiveModelRenderer's geometry map at render time.
    class PrimitiveModel : public Object
    {
      public:
        explicit PrimitiveModel(PrimitiveType Type);
        virtual ~PrimitiveModel() = default;

        const char *GetNodeTypeName() const override;

        DEFINE_MEMBER_CONST(PrimitiveType, PrimitiveType);

        // Appearance
        DEFINE_MEMBER(QVector3D, Color, QVector3D(1.0f, 1.0f, 1.0f));
        DEFINE_MEMBER(float, Opacity, 1.0f);

        // Screen-space line thickness in pixels (Circle and Line only)
        DEFINE_MEMBER(float, Thickness, 2.0f);
    };

    using PrimitiveModelPtr = std::unique_ptr<PrimitiveModel>;

} // namespace Canavar::Engine
