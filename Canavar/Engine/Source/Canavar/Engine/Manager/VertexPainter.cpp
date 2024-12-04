#include "VertexPainter.h"

#include "Canavar/Engine/Manager/NodeManager.h"
#include "Canavar/Engine/Manager/RenderingManager/RenderingManager.h"

Canavar::Engine::VertexPainter::VertexPainter(QObject *parent)
    : Manager(parent)
{}

void Canavar::Engine::VertexPainter::Initialize()
{
    initializeOpenGLFunctions();

    mVertexPainterShader = new Shader(ShaderType::VertexPainter, "Vertex Painter Shader");
    mVertexPainterShader->AddPath(QOpenGLShader::Compute, ":/Resources/Shaders/VertexPainter.comp");
    mVertexPainterShader->Initialize();
}

void Canavar::Engine::VertexPainter::PostInitialize()
{
    mRendererManager = mManagerProvider->GetRenderingManager();
    mNodeManager = mManagerProvider->GetNodeManager();
}

void Canavar::Engine::VertexPainter::Update(float)
{
    if (mVertexPaintingEnabled == false)
    {
        return;
    }

    if (mMousePressed == false)
    {
        return;
    }

    const auto x = mCurrentMousePosition.x();
    const auto y = mCurrentMousePosition.y();

    mNodeInfo = mRendererManager->FetchNodeInfoFromScreenCoordinates(x, y);

    if (mNodeInfo.success)
    {
        if (const auto pModel = mNodeManager->GetNodeById<Model>(mNodeInfo.nodeId))
        {
            mTargetMesh = mNodeManager->GetMeshById(pModel, mNodeInfo.meshId).get();
        }
    }

    if (mTargetMesh == nullptr)
    {
        return;
    }

    mBrushCenter = mRendererManager->FetchFragmentLocalPositionFromScreen(x, y);

    mVertexPainterShader->Bind();
    mVertexPainterShader->SetUniformValue("brush.center", mBrushCenter);
    mVertexPainterShader->SetUniformValue("brush.radius", mBrushRadius);
    mVertexPainterShader->SetUniformValue("brush.color", mBrushColor);
    glBindVertexArray(mTargetMesh->GetVAO());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mTargetMesh->GetVBO());
    glDispatchCompute(mTargetMesh->GetVertices().size(), 1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
    glBindVertexArray(0);
    mVertexPainterShader->Release();
}

void Canavar::Engine::VertexPainter::OnMousePressed(QMouseEvent *pEvent)
{
    if (pEvent->button() == Qt::LeftButton)
    {
        mMousePressed = true;
    }
}

void Canavar::Engine::VertexPainter::OnMouseReleased(QMouseEvent *pEvent)
{
    if (pEvent->button() == Qt::LeftButton)
    {
        mMousePressed = false;
    }
}

void Canavar::Engine::VertexPainter::OnMouseMoved(QMouseEvent *pEvent)
{
    mCurrentMousePosition = pEvent->position();
}

void Canavar::Engine::VertexPainter::OnWheelMoved(QWheelEvent *pEvent)
{
    if (pEvent->angleDelta().y() < 0)
    {
        mBrushRadius += 0.05;
    }
    else if (pEvent->angleDelta().y() > 0)
    {
        mBrushRadius += 0.05;
    }
}
