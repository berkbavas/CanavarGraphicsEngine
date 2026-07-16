#pragma once

#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/Manager/Manager.h"
#include "Canavar/Engine/Camera/PerspectiveCamera.h"
#include "Canavar/Engine/Scene/Scene.h"

namespace Canavar::Engine
{
    class Renderer;
    class NodeManager;
    class TexturedModel;
    class LightManager;

    class TexturedModelRenderer : public Manager
    {
      public:
        explicit TexturedModelRenderer(Renderer* pRenderer);
        virtual ~TexturedModelRenderer() = default;

        void Initialize() override;
        void Render(RenderPass RenderPass) override;
        const QMap<QString, ScenePtr>& GetScenes() const;

      private:
        void SetCommonUniforms(RenderPass RenderPass, PerspectiveCamera* pActiveCamera);
        void RenderModels(RenderPass RenderPass, PerspectiveCamera* pActiveCamera, bool OverlayPass);
        bool ShouldRender(TexturedModel* pTexturedModel, bool OverlayPass) const;

        Renderer* mRenderer{ nullptr };
        NodeManager* mNodeManager{ nullptr };
        LightManager* mLightManager{ nullptr };

        QMap<QString, ScenePtr> mScenes;
        ShaderPtr mTexturedModelShader{ nullptr };
    };

    using TexturedModelRendererPtr = std::unique_ptr<TexturedModelRenderer>;

}