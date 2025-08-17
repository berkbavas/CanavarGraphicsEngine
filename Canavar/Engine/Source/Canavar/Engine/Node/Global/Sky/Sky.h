#pragma once

#include "Canavar/Engine/Node/Global/Global.h"
#include "Canavar/Engine/Primitive/Quad.h"
#include "Canavar/Engine/Util/Macros.h"

#include <QOpenGLExtraFunctions>
#include <QVector3D>

namespace Canavar::Engine
{
    class Shader;
    class DirectionalLight;
    class Camera;

    class Sky : public Global, protected QOpenGLExtraFunctions
    {
      public:
        CANAVAR_NODE(Sky);

        void Initialize();

        void Render(Shader* pShader, DirectionalLight* pSun, Camera* pCamera);

      private:
        Quad* mQuad;

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

    using SkyPtr = std::shared_ptr<Sky>;
}
