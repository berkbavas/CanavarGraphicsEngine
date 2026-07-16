#pragma once

#include "Canavar/Engine/Core/Quad.h"
#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/PostProcessEffect/PostProcessEffect.h"
#include "Canavar/Engine/Util/Macros.h"

namespace Canavar::Engine
{
    class AcesEffect : public PostProcessEffect
    {
      public:
        AcesEffect();
        virtual ~AcesEffect() = default;

        void ApplyEffect(Framebuffer* pInput, Framebuffer* pOutput) override;

      private:
        QuadPtr mQuad{ nullptr };
        ShaderPtr mShader{ nullptr };

        DEFINE_MEMBER(float, Exposure, 1.0f);
    };

    using AcesEffectPtr = std::unique_ptr<AcesEffect>;
}