#pragma once

#include "Canavar/Engine/Core/Quad.h"
#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/GlobalNode/GlobalNode.h"
#include "Canavar/Engine/Util/Macros.h"

namespace Canavar::Engine
{
    class PerspectiveCamera;
    class DirectionalLight;

    class Sky : public GlobalNode, protected QOpenGLFunctions_4_5_Core
    {
      public:
        Sky();
        virtual ~Sky() = default;

        const char *GetNodeTypeName() const override { return "Sky"; }

        void Render(PerspectiveCamera *pActiveCamera, DirectionalLight *pSun);

      private:
        ShaderPtr mSkyShader{ nullptr };
        QuadPtr mQuad{ nullptr };

        DEFINE_MEMBER(float, SunIntensity, 22.0f);
        DEFINE_MEMBER(float, PlanetRadius, 6371e3f);
        DEFINE_MEMBER(float, AtmosphereRadius, 6471e3f);
        DEFINE_MEMBER(QVector3D, BetaRayleigh, QVector3D(5.5e-6, 13.0e-6, 22.4e-6));
        DEFINE_MEMBER(float, BetaMie, 21e-6f);
        DEFINE_MEMBER(float, ScaleHeightR, 8.0e3f);
        DEFINE_MEMBER(float, ScaleHeightM, 1.2e3f);
        DEFINE_MEMBER(float, MieG, 0.8f);
        DEFINE_MEMBER(float, HorizonOffset, 0.15f);
        DEFINE_MEMBER(bool, Enabled, true);
    };

    using SkyPtr = std::unique_ptr<Sky>;
}