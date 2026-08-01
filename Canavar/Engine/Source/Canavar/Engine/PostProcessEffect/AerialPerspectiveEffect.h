#pragma once

#include "Canavar/Engine/Core/Quad.h"
#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/PostProcessEffect/PostProcessEffect.h"
#include "Canavar/Engine/Util/Macros.h"

#include <QVector3D>

namespace Canavar::Engine
{
    // Aerial perspective: blends scene objects into the atmospheric color based on
    // world-space distance. Uses the same Rayleigh+Mie model as Sky.frag so that
    // the atmosphere transitions seamlessly from sky background to scene objects.
    //
    // Call Set*() methods each frame before ApplyEffect(); the Renderer does this automatically.
    class AerialPerspectiveEffect : public PostProcessEffect
    {
      public:
        AerialPerspectiveEffect();
        ~AerialPerspectiveEffect() override = default;

        void ApplyEffect(Framebuffer* pInput, Framebuffer* pOutput) override;

        // Per-frame data supplied by the Renderer
        void SetWorldPositionTexture(GLuint Texture) { mWorldPositionTexture = Texture; }
        void SetCameraPosition(const QVector3D& Pos) { mCameraPosition = Pos; }
        void SetSunDirection(const QVector3D& Dir) { mSunDirection = Dir; }
        void SetSunIntensity(float V) { mSunIntensity = V; }
        void SetPlanetRadius(float V) { mPlanetRadius = V; }
        void SetAtmosphereRadius(float V) { mAtmosphereRadius = V; }
        void SetBetaRayleigh(const QVector3D& V) { mBetaRayleigh = V; }
        void SetBetaMie(float V) { mBetaMie = V; }
        void SetScaleHeightR(float V) { mScaleHeightR = V; }
        void SetScaleHeightM(float V) { mScaleHeightM = V; }
        void SetMieG(float V) { mMieG = V; }

        // Distance-to-atmosphere density; ~1e-5 is a good starting point for km-scale scenes
        DEFINE_MEMBER(float, Density, 1e-5f);

      private:
        QuadPtr mQuad{ nullptr };
        ShaderPtr mShader{ nullptr };

        GLuint mWorldPositionTexture{ 0 };
        QVector3D mCameraPosition{};
        QVector3D mSunDirection{ 0.0f, 1.0f, 0.0f };
        float mSunIntensity{ 22.0f };
        float mPlanetRadius{ 6371e3f };
        float mAtmosphereRadius{ 6471e3f };
        QVector3D mBetaRayleigh{ 5.5e-6f, 13.0e-6f, 22.4e-6f };
        float mBetaMie{ 21e-6f };
        float mScaleHeightR{ 8.0e3f };
        float mScaleHeightM{ 1.2e3f };
        float mMieG{ 0.8f };
    };

    using AerialPerspectiveEffectPtr = std::unique_ptr<AerialPerspectiveEffect>;
}
