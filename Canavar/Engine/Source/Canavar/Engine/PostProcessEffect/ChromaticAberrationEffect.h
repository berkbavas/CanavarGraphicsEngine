#pragma once

#include "Canavar/Engine/Core/Quad.h"
#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/PostProcessEffect/PostProcessEffect.h"
#include "Canavar/Engine/Util/Macros.h"

namespace Canavar::Engine
{
    class ChromaticAberrationEffect : public PostProcessEffect
    {
      public:
        ChromaticAberrationEffect();
        virtual ~ChromaticAberrationEffect() = default;

        void ApplyEffect(Framebuffer* pInput, Framebuffer* pOutput) override;

      private:
        QuadPtr mQuad{ nullptr };
        ShaderPtr mShader{ nullptr };

        DEFINE_MEMBER(float, Strength, 0.003f);
    };

    using ChromaticAberrationEffectPtr = std::unique_ptr<ChromaticAberrationEffect>;
}