#include "TexturedModelRenderer.h"

#include "Canavar/Engine/Manager/LightManager.h"
#include "Canavar/Engine/Manager/NodeManager.h"
#include "Canavar/Engine/Manager/Renderer.h"
#include "Canavar/Engine/Model/TexturedModel/TexturedModel.h"
#include "Canavar/Engine/Util/AssimpModelImporter.h"

Canavar::Engine::TexturedModelRenderer::TexturedModelRenderer(Renderer *pRenderer)
    : mRenderer(pRenderer)
{}

void Canavar::Engine::TexturedModelRenderer::Initialize()
{
    mNodeManager = mRenderer->GetNodeManager();
    mLightManager = mRenderer->GetLightManager();

    mScenes = AssimpModelImporter::Import(Canavar::Engine::MODELS_FOLDER, { "*.obj", "*.glb", "*.gltf", "*.ply" });

    mTexturedModelShader = std::make_unique<Shader>("Textured Model Shader");
    mTexturedModelShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/TexturedModel.vert");
    mTexturedModelShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/TexturedModel.frag");
    mTexturedModelShader->Initialize();
}

void Canavar::Engine::TexturedModelRenderer::Render(RenderPass RenderPass)
{
    RenderModels(RenderPass, mRenderer->GetActiveCamera(), false);
}

void Canavar::Engine::TexturedModelRenderer::RenderOverlay(RenderPass RenderPass)
{
    RenderModels(RenderPass, mRenderer->GetActiveCamera(), true);
}

const QMap<QString, Canavar::Engine::ScenePtr> &Canavar::Engine::TexturedModelRenderer::GetScenes() const
{
    return mScenes;
}

Canavar::Engine::Scene *Canavar::Engine::TexturedModelRenderer::GetSceneByName(const QString &SceneName) const
{
    return mScenes.value(SceneName, nullptr).get();
}

void Canavar::Engine::TexturedModelRenderer::RenderModels(RenderPass RenderPass, PerspectiveCamera *pActiveCamera, bool OverlayPass)
{
    SetCommonUniforms(RenderPass, pActiveCamera);

    const auto &TexturedModels = mNodeManager->GetTexturedModels();
    Shader *pTexturedModelShader = mTexturedModelShader.get();
    const auto PointLightsRadius = pActiveCamera->GetZFar();

    for (const auto &pTexturedModel : TexturedModels)
    {
        if (!ShouldRender(pTexturedModel.get(), OverlayPass))
        {
            continue; // Skip rendering this textured model based on visibility and overlay settings
        }

        const auto &ModelName = pTexturedModel->GetModelName();

        if (Scene *pScene = GetSceneByName(ModelName))
        {
            mLightManager->SetPointLightsUniforms(pTexturedModelShader, pTexturedModel->GetPosition(), PointLightsRadius);
            pTexturedModel->Render(pScene, pTexturedModelShader, RenderPass);
        }
        else
        {
            CGE_EXIT_FAILURE("TexturedModelRenderer::Render: Scene '{}' not found for textured model '{}'.", ModelName.toStdString(), pTexturedModel->GetModelUniqueNameStdString());
        }
    }
}

void Canavar::Engine::TexturedModelRenderer::SetCommonUniforms(RenderPass RenderPass, PerspectiveCamera *pActiveCamera)
{
    mLightManager->SetDirectionalLightsUniforms(mTexturedModelShader.get());
    mRenderer->GetHaze()->SetUniforms(mTexturedModelShader.get());

    mTexturedModelShader->Bind();
    mTexturedModelShader->SetUniform("uVP", pActiveCamera->GetViewProjectionMatrix());
    mTexturedModelShader->SetUniform("uFar", pActiveCamera->GetZFar());
    mTexturedModelShader->SetUniform("uCameraPosition", pActiveCamera->GetPosition());
    mTexturedModelShader->Unbind();
}

bool Canavar::Engine::TexturedModelRenderer::ShouldRender(TexturedModel *pTexturedModel, bool OverlayPass) const
{
    if (pTexturedModel->GetVisible() == false)
    {
        return false; // Skip invisible textured models
    }

    if (pTexturedModel->IsFuzzyTransparent())
    {
        return false; // Skip almost transparent textured models
    }

    if (OverlayPass && !pTexturedModel->GetOverlay())
    {
        return false; // Skip non-overlay textured models in overlay pass
    }

    if (!OverlayPass && pTexturedModel->GetOverlay())
    {
        return false; // Skip overlay textured models in non-overlay pass
    }

    return true; // Render the textured model if it passed all checks
}
