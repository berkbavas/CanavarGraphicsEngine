#pragma once

#include "Canavar/Engine/Camera/PerspectiveCamera.h"
#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/Manager/Manager.h"
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
        void Render(RenderPass RenderPass, PerspectiveCamera* pActiveCamera) override;
        const QMap<QString, ScenePtr>& GetScenes() const;
        Scene* GetSceneByName(const QString& SceneName) const;

        // Sets the node/mesh IDs to highlight in the next rendered frame.
        void SetSelectionState(int SelectedNodeId, int SelectedMeshId);

      private:
        void SetCommonUniforms(RenderPass RenderPass, PerspectiveCamera* pActiveCamera);
        void RenderModels(RenderPass RenderPass, PerspectiveCamera* pActiveCamera);
        bool ShouldRender(TexturedModel* pTexturedModel) const;

        Renderer* mRenderer{ nullptr };
        NodeManager* mNodeManager{ nullptr };
        LightManager* mLightManager{ nullptr };

        QMap<QString, ScenePtr> mScenes;
        ShaderPtr mTexturedModelShader{ nullptr };

        int mSelectedNodeId{ -1 };
        int mSelectedMeshId{ -1 };
    };

    using TexturedModelRendererPtr = std::unique_ptr<TexturedModelRenderer>;

}