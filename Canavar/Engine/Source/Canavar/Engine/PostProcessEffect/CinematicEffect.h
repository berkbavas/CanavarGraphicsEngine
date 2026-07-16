#pragma once
#include "Canavar/Engine/Core/Quad.h"
#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/PostProcessEffect/PostProcessEffect.h"
#include "Canavar/Engine/Util/Macros.h"

#include <QDateTime>

namespace Canavar::Engine
{
    class CinematicEffect : public PostProcessEffect
    {
      public:
        CinematicEffect();
        virtual ~CinematicEffect() = default;

        void ApplyEffect(Framebuffer* pInput, Framebuffer* pOutput) override;

      private:
        void UpdateTime();

        QuadPtr mQuad{ nullptr };
        ShaderPtr mShader{ nullptr };

        qint64 mLastTime{ QDateTime::currentMSecsSinceEpoch() };

        float mTime{ 0.0f };

        DEFINE_MEMBER(float, VignetteRadius, 0.95f);
        DEFINE_MEMBER(float, VignetteSoftness, 0.5f);
        DEFINE_MEMBER(float, GrainStrength, 0.00f);
    };

    using CinematicEffectPtr = std::unique_ptr<CinematicEffect>;
}