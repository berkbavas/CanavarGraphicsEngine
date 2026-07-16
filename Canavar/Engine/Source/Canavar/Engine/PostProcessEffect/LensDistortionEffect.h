#pragma once

#include "Canavar/Engine/Core/Quad.h"
#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/PostProcessEffect/PostProcessEffect.h"
#include "Canavar/Engine/Util/Macros.h"

namespace Canavar::Engine
{
    class LensDistortionEffect : public PostProcessEffect
    {
      public:
        LensDistortionEffect();
        virtual ~LensDistortionEffect() = default;

        void ApplyEffect(Framebuffer* pInput, Framebuffer* pOutput) override;

      private:
        QuadPtr mQuad{ nullptr };
        ShaderPtr mShader{ nullptr };

        DEFINE_MEMBER(float, Barrel, 0.15f); // >0 barrel, <0 pincushion
        DEFINE_MEMBER(float, Zoom, 0.9f);    // compensating zoom
    };

    using LensDistortionEffectPtr = std::unique_ptr<LensDistortionEffect>;
}