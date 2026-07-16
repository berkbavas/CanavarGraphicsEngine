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
    mGeometries[PrimitiveType::Circle] = std::make_unique<CircleGeometry>();
    mGeometries[PrimitiveType::Disk] = std::make_unique<DiskGeometry>();
    mGeometries[PrimitiveType::Line] = std::make_unique<LineGeometry>();
    mGeometries[PrimitiveType::Plane] = std::make_unique<PlaneGeometry>();
    mGeometries[PrimitiveType::Sphere] = std::make_unique<SphereGeometry>();

    for (auto &[Type, pGeometry] : mGeometries)
    {
        pGeometry->Initialize();
    }

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
}

void Canavar::Engine::PrimitiveModelRenderer::Render(RenderPass RenderPass)
{
    auto *pCamera = mRenderer->GetActiveCamera();
    if (!pCamera)
    {
        return;
    }

    // Solid primitives respect the opaque / transparent render pass
    RenderSolidPrimitives(RenderPass, pCamera);

    // Line primitives are always blended (transparent), rendered only on the transparent pass
    if (RenderPass == RenderPass::Transparent)
    {
        RenderLinePrimitives(pCamera);
    }
}

void Canavar::Engine::PrimitiveModelRenderer::Resize(int Width, int Height)
{
    mWidth = Width > 0 ? Width : 1;
    mHeight = Height > 0 ? Height : 1;
}

// ─────────────────────────────────────────────────────────────────────────────

void Canavar::Engine::PrimitiveModelRenderer::SetSolidCommonUniforms(PerspectiveCamera *pCamera)
{
    mPrimitiveShader->Bind();
    mPrimitiveShader->SetUniform("uVP", pCamera->GetViewProjectionMatrix());
    mPrimitiveShader->SetUniform("uFar", pCamera->GetZFar());
    mPrimitiveShader->SetUniform("uCameraPosition", pCamera->GetPosition());
    mPrimitiveShader->Unbind();
}

void Canavar::Engine::PrimitiveModelRenderer::SetLineCommonUniforms(PerspectiveCamera *pCamera)
{
    mCircleLineShader->Bind();
    mCircleLineShader->SetUniform("uVP", pCamera->GetViewProjectionMatrix());
    mCircleLineShader->SetUniform("uFar", pCamera->GetZFar());
    mCircleLineShader->SetUniform("uViewportSize", QVector2D(static_cast<float>(mWidth), static_cast<float>(mHeight)));
    mCircleLineShader->Unbind();
}

void Canavar::Engine::PrimitiveModelRenderer::RenderSolidPrimitives(RenderPass RenderPass, PerspectiveCamera *pCamera)
{
    SetSolidCommonUniforms(pCamera);

    mPrimitiveShader->Bind();

    for (const auto &pModel : mNodeManager->GetPrimitiveModels())
    {
        const auto Type = pModel->GetPrimitiveType();

        // Skip line primitives in this pass
        if (Type == PrimitiveType::Circle || Type == PrimitiveType::Line)
        {
            continue;
        }

        if (!pModel->GetVisible())
        {
            continue;
        }

        const float Opacity = pModel->GetOpacity();

        const bool IsTransparent = Opacity < 1.0f - 1e-4f;
        if (IsTransparent && RenderPass == RenderPass::Opaque)
        {
            continue;
        }
        if (!IsTransparent && RenderPass == RenderPass::Transparent)
        {
            continue;
        }

        mPrimitiveShader->SetUniform("uModelMatrix", pModel->GetTransformation());
        mPrimitiveShader->SetUniform("uNormalMatrix", pModel->GetTransformation().normalMatrix());
        mPrimitiveShader->SetUniform("uColor", pModel->GetColor());
        mPrimitiveShader->SetUniform("uOpacity", Opacity);
        mPrimitiveShader->SetUniform("uNodeId", pModel->GetNodeId());

        mGeometries.at(Type)->Render();
    }

    mPrimitiveShader->Unbind();
}

void Canavar::Engine::PrimitiveModelRenderer::RenderLinePrimitives(PerspectiveCamera *pCamera)
{
    SetLineCommonUniforms(pCamera);

    mCircleLineShader->Bind();

    for (const auto &pModel : mNodeManager->GetPrimitiveModels())
    {
        const auto Type = pModel->GetPrimitiveType();

        if (Type != PrimitiveType::Circle && Type != PrimitiveType::Line)
        {
            continue;
        }

        if (!pModel->GetVisible())
        {
            continue;
        }

        mCircleLineShader->SetUniform("uModelMatrix", pModel->GetTransformation());
        mCircleLineShader->SetUniform("uColor", pModel->GetColor());
        mCircleLineShader->SetUniform("uOpacity", pModel->GetOpacity());
        mCircleLineShader->SetUniform("uThickness", pModel->GetThickness());
        mCircleLineShader->SetUniform("uNodeId", pModel->GetNodeId());

        mGeometries.at(Type)->Render();
    }

    mCircleLineShader->Unbind();
}
