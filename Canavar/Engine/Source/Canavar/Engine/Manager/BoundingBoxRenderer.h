#pragma once

#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/Manager/Manager.h"
#include "Canavar/Engine/Util/Macros.h"

#include <memory>

#include <QOpenGLFunctions_4_5_Core>

namespace Canavar::Engine
{
    class Renderer;
    class Object;
    class PerspectiveCamera;

    class BoundingBoxRenderer : public Manager, protected QOpenGLFunctions_4_5_Core
    {
      public:
        explicit BoundingBoxRenderer(Renderer *pRenderer);
        ~BoundingBoxRenderer() override;

        void Initialize() override;
        void Render(RenderPass RenderPass, PerspectiveCamera *pCamera) override;

      private:
        void RenderBoundingBoxes(PerspectiveCamera *pCamera);
        bool ShouldRenderBoundingBox(const Object *pObject, const PerspectiveCamera *pCamera) const;

        Renderer *mRenderer{ nullptr };

        ShaderPtr mShader{ nullptr };
        GLuint mVAO{ 0 };
        GLuint mVBO{ 0 };

        DEFINE_MEMBER(bool, RenderBoundingBoxes, true);
    };

    using BoundingBoxRendererPtr = std::unique_ptr<BoundingBoxRenderer>;
}