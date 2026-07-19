#include "BoundingBoxRenderer.h"

#include "Canavar/Engine/Camera/PerspectiveCamera.h"
#include "Canavar/Engine/Manager/NodeManager.h"
#include "Canavar/Engine/Manager/Renderer.h"
#include "Canavar/Engine/Object/Object.h"

Canavar::Engine::BoundingBoxRenderer::BoundingBoxRenderer(Renderer *pRenderer)
    : mRenderer(pRenderer)
{}

Canavar::Engine::BoundingBoxRenderer::~BoundingBoxRenderer()
{
    if (mVAO)
    {
        glDeleteVertexArrays(1, &mVAO);
    }

    if (mVBO)
    {
        glDeleteBuffers(1, &mVBO);
    }
}

void Canavar::Engine::BoundingBoxRenderer::Initialize()
{
    initializeOpenGLFunctions();

    mShader = std::make_unique<Shader>("BoundingBox Shader");
    mShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/BoundingBox.vert");
    mShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/BoundingBox.frag");
    mShader->Initialize();

    // 12 edges × 2 endpoints = 24 vertices, each with 3 floats (world-space positions)
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, 24 * 3 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void Canavar::Engine::BoundingBoxRenderer::Render(RenderPass RenderPass, PerspectiveCamera *pCamera)
{
    switch (RenderPass)
    {
    case RenderPass::Opaque:
        RenderBoundingBoxes(pCamera);
        break;
    default:
        break;
    }
}

void Canavar::Engine::BoundingBoxRenderer::RenderBoundingBoxes(PerspectiveCamera *pCamera)
{
    if (!mRenderBoundingBoxes)
        return;

    const auto &Objects = mRenderer->GetNodeManager()->GetObjects();

    mShader->Bind();
    mShader->SetUniform("uVP", pCamera->GetViewProjectionMatrix());
    mShader->SetUniform("uFar", pCamera->GetZFar());
    mShader->SetUniform("uColor", QVector3D(0.0f, 1.0f, 0.0f));

    glBindVertexArray(mVAO);

    // Edge index pairs for the 12 edges of a box (indices into Corners[8])
    static constexpr int EdgeIndices[24] = {
        0, 1, 1, 2, 2, 3, 3, 0, // bottom face
        4, 5, 5, 6, 6, 7, 7, 4, // top face
        0, 4, 1, 5, 2, 6, 3, 7, // vertical edges
    };

    for (const auto *pObject : Objects)
    {
        if (!ShouldRenderBoundingBox(pObject, pCamera))
        {
            continue;
        }

        const AABB &LocalAABB = pObject->GetAABB();
        const QMatrix4x4 World = pObject->GetWorldTransformation();
        const QVector3D &Min = LocalAABB.GetMin();
        const QVector3D &Max = LocalAABB.GetMax();

        // 8 corners in local space, transformed to world space
        const QVector3D LocalCorners[8] = {
            { Min.x(), Min.y(), Min.z() }, //
            { Max.x(), Min.y(), Min.z() }, //
            { Max.x(), Max.y(), Min.z() }, //
            { Min.x(), Max.y(), Min.z() }, //
            { Min.x(), Min.y(), Max.z() }, //
            { Max.x(), Min.y(), Max.z() }, //
            { Max.x(), Max.y(), Max.z() }, //
            { Min.x(), Max.y(), Max.z() }, //
        };

        QVector3D WorldCorners[8];
        for (int i = 0; i < 8; ++i)
        {
            WorldCorners[i] = (World * QVector4D(LocalCorners[i], 1.0f)).toVector3D();
        }

        // Build 24 world-space line endpoints
        float LineVerts[24 * 3];
        for (int i = 0; i < 24; ++i)
        {
            const QVector3D &P = WorldCorners[EdgeIndices[i]];
            LineVerts[i * 3 + 0] = P.x();
            LineVerts[i * 3 + 1] = P.y();
            LineVerts[i * 3 + 2] = P.z();
        }

        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(LineVerts), LineVerts);

        glDrawArrays(GL_LINES, 0, 24);
    }

    glBindVertexArray(0);
    mShader->Unbind();
}

bool Canavar::Engine::BoundingBoxRenderer::ShouldRenderBoundingBox(const Object *pObject, const PerspectiveCamera *pCamera) const
{
    if (!pObject->GetVisible())
    {
        return false;
    }

    if (pObject == pCamera)
    {
        return false;
    }

    if (dynamic_cast<const Canavar::Engine::PrimitiveModel *>(pObject))
    {
        return false;
    }

    return true;
}
