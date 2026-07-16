#pragma once

#include <memory>

#include <QOpenGLFunctions_4_5_Core>

namespace Canavar::Engine
{
    class Framebuffer;

    class PostProcessEffect : protected QOpenGLFunctions_4_5_Core
    {
      public:
        PostProcessEffect();
        virtual ~PostProcessEffect() = default;

        virtual void ApplyEffect(Framebuffer* Input, Framebuffer* Output) = 0;
    };

    using PostProcessEffectPtr = std::unique_ptr<PostProcessEffect>;
}