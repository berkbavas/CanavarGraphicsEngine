#pragma once

#include "Canavar/Engine/Core/Enums.h"
#include "Canavar/Engine/Object/Object.h"
#include "Canavar/Engine/Util/Macros.h"

#include <memory>

#include <QVector3D>

namespace Canavar::Engine
{
    class PerspectiveCamera;
    class GeometryBase;
    class Shader;

    // Base class for all procedurally-generated primitive models.
    // The PrimitiveType enum passed to the constructor identifies which geometry
    // will be fetched from PrimitiveModelRenderer's geometry map at render time.
    class PrimitiveModel : public Object
    {
      public:
        explicit PrimitiveModel(PrimitiveType Type);
        virtual ~PrimitiveModel() = default;

        virtual void Render(GeometryBase *pGeometry, Shader *pShader, PerspectiveCamera *pCamera);
        const char *GetNodeTypeName() const override;

        bool HasTransparency() const;
        bool IsFullyOpaque() const;

        // Type of primitive (Circle, Disk, Line, Plane, Sphere)
        DEFINE_MEMBER_CONST(PrimitiveType, PrimitiveType);

        // Appearance
        DEFINE_MEMBER(QVector3D, Color, QVector3D(1.0f, 1.0f, 1.0f));
        DEFINE_MEMBER(float, Opacity, 1.0f);

        // Renders on top of everything else
        DEFINE_MEMBER(bool, Overlay, false);
    };

    using PrimitiveModelPtr = std::unique_ptr<PrimitiveModel>;

} // namespace Canavar::Engine
