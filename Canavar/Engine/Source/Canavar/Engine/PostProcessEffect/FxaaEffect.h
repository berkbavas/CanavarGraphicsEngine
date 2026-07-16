#pragma once

#include "Canavar/Engine/Core/Quad.h"
#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/PostProcessEffect/PostProcessEffect.h"
#include "Canavar/Engine/Util/Macros.h"

namespace Canavar::Engine
{
    class FxaaEffect : public PostProcessEffect
    {
      public:
        FxaaEffect();
        virtual ~FxaaEffect() = default;

        void ApplyEffect(Framebuffer* pInput, Framebuffer* pOutput) override;

      private:
        QuadPtr mQuad{ nullptr };
        ShaderPtr mShader{ nullptr };

        DEFINE_MEMBER(float, SubpixelQuality, 0.75f);
        DEFINE_MEMBER(float, EdgeThreshold, 0.125f);
        DEFINE_MEMBER(float, EdgeThresholdMin, 0.0312f);
    };

    using FxaaEffectPtr = std::unique_ptr<FxaaEffect>;
}