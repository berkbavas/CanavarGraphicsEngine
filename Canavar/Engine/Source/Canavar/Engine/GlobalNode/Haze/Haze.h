#pragma once

#include "Canavar/Engine/GlobalNode/GlobalNode.h"
#include "Canavar/Engine/Util/Macros.h"

namespace Canavar::Engine
{
    class Shader;

    class Haze : public GlobalNode
    {
      public:
        Haze();
        virtual ~Haze() = default;

        const char *GetNodeTypeName() const override { return "Haze"; }

        void SetUniforms(Shader *pShader) const;

        DEFINE_MEMBER(QVector3D, Color, QVector3D(0.33f, 0.38f, 0.47f));
        DEFINE_MEMBER(float, Density, 1.0f);
        DEFINE_MEMBER(float, Gradient, 1.5f);
        DEFINE_MEMBER(bool, Enabled, true);
    };

    using HazePtr = std::unique_ptr<Haze>;
}