#include "PrimitiveModelRenderer.h"

#include "Canavar/Engine/Camera/PerspectiveCamera.h"
#include "Canavar/Engine/Manager/NodeManager.h"
#include "Canavar/Engine/Manager/Renderer.h"
#include "Canavar/Engine/Model/PrimitiveModel/Geometry/CircleGeometry.h"
#include "Canavar/Engine/Model/PrimitiveModel/Geometry/DiskGeometry.h"
#include "Canavar/Engine/Model/PrimitiveModel/Geometry/LineGeometry.h"
#include "Canavar/Engine/Model/PrimitiveModel/Geometry/PlaneGeometry.h"
#include "Canavar/Engine/Model/PrimitiveModel/Geometry/SphereGeometry.h"
#include "Canavar/Engine/Model/PrimitiveModel/PrimitiveModel.h"

Canavar::Engine::PrimitiveModelRenderer::PrimitiveModelRenderer(Renderer *pRenderer)
    : mRenderer(pRenderer)
{}

void Canavar::Engine::PrimitiveModelRenderer::Initialize()
{
    mNodeManager = mRenderer->GetNodeManager();

    // ── Create geometry for each primitive type ───────────────────────────────
    mGeometries[PrimitiveType::Circle] = std::make_unique<CircleGeometry>(96);
    mGeometries[PrimitiveType::Disk] = std::make_unique<DiskGeometry>(1.0f, 96);
    mGeometries[PrimitiveType::Line] = std::make_unique<LineGeometry>();
    mGeometries[PrimitiveType::Plane] = std::make_unique<PlaneGeometry>();
    mGeometries[PrimitiveType::Sphere] = std::make_unique<SphereGeometry>(1.0f, 96, 96);

    // ── Primitive shader (Disk, Plane, Sphere) ────────────────────────────────
    mPrimitiveShader = std::make_unique<Shader>("Primitive Shader");
    mPrimitiveShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Primitive.vert");
    mPrimitiveShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Primitive.frag");
    mPrimitiveShader->Initialize();

    // ── CircleLine shader (Circle, Line) ─────────────────────────────────────
    mCircleLineShader = std::make_unique<Shader>("CircleLine Shader");
    mCircleLineShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/CircleLine.vert");
    mCircleLineShader->AddPath(QOpenGLShader::Geometry, ":/Resources/Shaders/CircleLine.geom");
    mCircleLineShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/CircleLine.frag");
    mCircleLineShader->Initialize();

    mShaders[PrimitiveType::Circle] = mCircleLineShader.get();
    mShaders[PrimitiveType::Line] = mCircleLineShader.get();
    mShaders[PrimitiveType::Disk] = mPrimitiveShader.get();
    mShaders[PrimitiveType::Plane] = mPrimitiveShader.get();
    mShaders[PrimitiveType::Sphere] = mPrimitiveShader.get();
}

void Canavar::Engine::PrimitiveModelRenderer::Render(RenderPass RenderPass, PerspectiveCamera *pCamera)
{
    SetCommonUniforms(pCamera);
    RenderPrimitiveModels(RenderPass, false, pCamera);
}

void Canavar::Engine::PrimitiveModelRenderer::RenderOverlay(RenderPass RenderPass, PerspectiveCamera *pCamera)
{
    SetCommonUniforms(pCamera);
    RenderPrimitiveModels(RenderPass, true, pCamera);
}

void Canavar::Engine::PrimitiveModelRenderer::RenderPrimitiveModels(RenderPass RenderPass, bool OverlayPass, PerspectiveCamera *pCamera)
{
    for (const auto pPrimitiveModel : mNodeManager->GetPrimitiveModels())
    {
        if (!ShouldRenderPrimitiveModel(pPrimitiveModel, RenderPass, OverlayPass))
        {
            continue; // Skip this primitive model if it shouldn't be rendered
        }

        PrimitiveType Type = pPrimitiveModel->GetPrimitiveType();
        GeometryBase *pGeometry = mGeometries[Type].get();
        Shader *pShader = mShaders[Type];

        pPrimitiveModel->Render(pGeometry, pShader, pCamera);
    }
}

bool Canavar::Engine::PrimitiveModelRenderer::ShouldRenderPrimitiveModel(PrimitiveModel *pPrimitiveModel, RenderPass RenderPass, bool OverlayPass) const
{
    if (pPrimitiveModel->GetVisible() == false)
    {
        return false; // Skip invisible primitive models
    }

    if (OverlayPass && !pPrimitiveModel->GetOverlay())
    {
        return false; // Skip non-overlay primitive models in overlay pass
    }

    if (!OverlayPass && pPrimitiveModel->GetOverlay())
    {
        return false; // Skip overlay primitive models in non-overlay pass
    }

    switch (RenderPass)
    {
    case RenderPass::Opaque:
    {
        if (pPrimitiveModel->HasTransparency())
        {
            return false; // Skip transparent primitive models in opaque pass
        }
        break;
    }
    case RenderPass::Transparent:
    {
        if (pPrimitiveModel->IsFullyOpaque())
        {
            return false; // Skip fully opaque primitive models in transparent pass
        }
        break;
    }
    }

    return true; // Render the primitive model if it passed all checks
}

void Canavar::Engine::PrimitiveModelRenderer::SetCommonUniforms(PerspectiveCamera *pCamera)
{
    mPrimitiveShader->Bind();
    mPrimitiveShader->SetUniform("uVP", pCamera->GetViewProjectionMatrix());
    mPrimitiveShader->SetUniform("uFar", pCamera->GetZFar());
    mPrimitiveShader->SetUniform("uCameraPosition", pCamera->GetPosition());
    mPrimitiveShader->Unbind();

    mCircleLineShader->Bind();
    mCircleLineShader->SetUniform("uVP", pCamera->GetViewProjectionMatrix());
    mCircleLineShader->SetUniform("uFar", pCamera->GetZFar());
    mCircleLineShader->SetUniform("uViewportSize", pCamera->GetViewportSize());
    mCircleLineShader->Unbind();
}
