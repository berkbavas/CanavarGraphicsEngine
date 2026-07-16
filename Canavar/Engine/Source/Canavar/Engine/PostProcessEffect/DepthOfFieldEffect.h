#pragma once

#include "Canavar/Engine/Core/Quad.h"
#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/PostProcessEffect/PostProcessEffect.h"
#include "Canavar/Engine/Util/Macros.h"

#include <QVector2D>
#include <QVector3D>

namespace Canavar::Engine
{
    // ─────────────────────────────────────────────────────────────────────────
    // DepthOfFieldEffect
    //
    // Screen-space depth-of-field using a Poisson-disc blur.
    // The blur radius for each pixel is derived from the Circle-of-Confusion
    // (CoC) computed from the world-position G-buffer attachment.
    //
    // Call SetWorldPositionTexture() and SetCameraPosition() every frame before
    // invoking ApplyEffect() (the Renderer does this automatically).
    // ─────────────────────────────────────────────────────────────────────────
    class DepthOfFieldEffect : public PostProcessEffect
    {
      public:
        DepthOfFieldEffect();
        ~DepthOfFieldEffect() override = default;

        void ApplyEffect(Framebuffer* pInput, Framebuffer* pOutput) override;

        // Per-frame data supplied by the Renderer
        void SetWorldPositionTexture(GLuint Texture) { mWorldPositionTexture = Texture; }
        void SetCameraPosition(const QVector3D& Position) { mCameraPosition = Position; }

      private:
        QuadPtr mQuad{ nullptr };
        ShaderPtr mShader{ nullptr };

        GLuint mWorldPositionTexture{ 0 };
        QVector3D mCameraPosition{};

        // ── Adjustable parameters ──────────────────────────────────────────
        DEFINE_MEMBER(float, FocusDistance, 100.0f); // World-space distance that is perfectly sharp
        DEFINE_MEMBER(float, FocusRange, 50.0f);     // ±half-width of the in-focus zone (world units)
        DEFINE_MEMBER(float, MaxBlurRadius, 15.0f);  // Maximum CoC blur radius in pixels
    };

    using DepthOfFieldEffectPtr = std::unique_ptr<DepthOfFieldEffect>;
}
