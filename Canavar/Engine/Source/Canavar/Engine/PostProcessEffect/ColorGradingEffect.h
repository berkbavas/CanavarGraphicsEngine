#pragma once

#include "Canavar/Engine/Core/Quad.h"
#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/PostProcessEffect/PostProcessEffect.h"
#include "Canavar/Engine/Util/Macros.h"

namespace Canavar::Engine
{
    class ColorGradingEffect : public PostProcessEffect
    {
      public:
        ColorGradingEffect();
        virtual ~ColorGradingEffect() = default;

        void ApplyEffect(Framebuffer* pInput, Framebuffer* pOutput) override;

      private:
        QuadPtr mQuad{ nullptr };
        ShaderPtr mShader{ nullptr };

        DEFINE_MEMBER(float, Brightness, 0.0f);
        DEFINE_MEMBER(float, Contrast, 1.0f);
        DEFINE_MEMBER(float, Saturation, 1.0f);
        DEFINE_MEMBER(float, Temperature, 0.0f);
    };

    using ColorGradingEffectPtr = std::unique_ptr<ColorGradingEffect>;
}