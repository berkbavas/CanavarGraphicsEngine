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

void Canavar::Engine::TexturedModelRenderer::Render(const RenderPassParameters &RenderPassParameters)
{
    RenderModels(RenderPassParameters, false);
}

const QMap<QString, Canavar::Engine::ScenePtr> &Canavar::Engine::TexturedModelRenderer::GetScenes() const
{
    return mScenes;
}

void Canavar::Engine::TexturedModelRenderer::SetCommonUniforms(const RenderPassParameters &RenderPassParameters)
{
    const auto &DirectionalLights = mNodeManager->GetDirectionalLights();

    const auto NumDirectionalLights = std::min(static_cast<int>(DirectionalLights.size()), MAX_DIRECTIONAL_LIGHTS);

    mTexturedModelShader->Bind();
    mTexturedModelShader->SetUniform("uNumDirectionalLights", NumDirectionalLights);

    int Index = 0;

    for (const auto &pDirectionalLight : DirectionalLights)
    {
        mTexturedModelShader->SetUniform(QString("uDirectionalLights[%1].Color").arg(Index), pDirectionalLight->GetColor());
        mTexturedModelShader->SetUniform(QString("uDirectionalLights[%1].Direction").arg(Index), pDirectionalLight->GetDirection().normalized());
        mTexturedModelShader->SetUniform(QString("uDirectionalLights[%1].Ambient").arg(Index), pDirectionalLight->GetAmbient());
        mTexturedModelShader->SetUniform(QString("uDirectionalLights[%1].Diffuse").arg(Index), pDirectionalLight->GetDiffuse());
        mTexturedModelShader->SetUniform(QString("uDirectionalLights[%1].Specular").arg(Index), pDirectionalLight->GetSpecular());
        mTexturedModelShader->SetUniform(QString("uDirectionalLights[%1].Radiance").arg(Index), pDirectionalLight->GetRadiance());
        ++Index;
    }

    const auto &pCamera = RenderPassParameters.pActiveCamera;
    
    mTexturedModelShader->SetUniform("uVP", pCamera->GetViewProjectionMatrix());
    mTexturedModelShader->SetUniform("uFar", pCamera->GetZFar());
    mTexturedModelShader->SetUniform("uCameraPosition", pCamera->GetPosition());
    mTexturedModelShader->Unbind();
}

void Canavar::Engine::TexturedModelRenderer::RenderModels(const RenderPassParameters &RenderPassParameters, bool OverlayPass)
{
    SetCommonUniforms(RenderPassParameters);

    const auto &TexturedModels = mNodeManager->GetTexturedModels();

    const auto ZFar = RenderPassParameters.pActiveCamera->GetZFar();

    for (const auto &pTexturedModel : TexturedModels)
    {
        if (ShouldRender(pTexturedModel.get(), OverlayPass))
        {
            const auto &ModelName = pTexturedModel->GetModelName();

            if (mScenes.contains(ModelName))
            {
                const auto &pScene = mScenes[ModelName];
                const auto &PointLights = mLightManager->GetPointLightsAround(pTexturedModel->GetPosition(), ZFar);

                pTexturedModel->Render(pScene.get(), mTexturedModelShader.get(), RenderPassParameters, PointLights);
            }
            else
            {
                CGE_EXIT_FAILURE("TexturedModelRenderer::Render: Scene '{}' not found for textured model '{}'.", ModelName.toStdString(), pTexturedModel->GetModelUniqueNameStdString());
            }
        }
    }
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
