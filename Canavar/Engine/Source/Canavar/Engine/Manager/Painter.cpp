#include "Painter.h"

#include "Canavar/Engine/Manager/NodeManager.h"
#include "Canavar/Engine/Manager/RenderingManager/RenderingManager.h"

Canavar::Engine::Painter::Painter(QObject *pParent)
    : Manager(pParent)
{}

void Canavar::Engine::Painter::PostInitialize()
{
    mRendererManager = mManagerProvider->GetRenderingManager();
    mNodeManager = mManagerProvider->GetNodeManager();
}

void Canavar::Engine::Painter::Update(float)
{
    if (mPainterEnabled == false)
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

    if (static_cast<bool>(mNodeInfo.Success))
    {
        if (const auto pModel = mNodeManager->FindNodeById<Model>(mNodeInfo.NodeId))
        {
            mTargetMesh = mNodeManager->GetMeshById(pModel, mNodeInfo.MeshId).get();
        }
    }

    if (mTargetMesh == nullptr)
    {
        return;
    }

    const auto [V0, V1, V2] = mTargetMesh->GetTriangleVertices(static_cast<unsigned int>(mNodeInfo.PrimitiveIndex));

    if (mPaint)
    {
        mTargetMesh->PaintVertex(V0, mBrushColor);
        mTargetMesh->PaintVertex(V1, mBrushColor);
        mTargetMesh->PaintVertex(V2, mBrushColor);
    }
    else
    {
        mTargetMesh->UnpaintVertex(V0);
        mTargetMesh->UnpaintVertex(V1);
        mTargetMesh->UnpaintVertex(V2);
    }
}

bool Canavar::Engine::Painter::MousePressed(QMouseEvent *pEvent)
{
    if (mPainterEnabled == false)
    {
        return false; // Do not consume the event if painter is disabled
    }

    if (pEvent->button() == Qt::RightButton)
    {
        mMousePressed = true;
        mPaint = !pEvent->modifiers().testFlag(Qt::ControlModifier);
        return true;
    }

    return false;
}

bool Canavar::Engine::Painter::MouseReleased(QMouseEvent *pEvent)
{
    if (mPainterEnabled == false)
    {
        return false; // Do not consume the event if painter is disabled
    }

    if (pEvent->button() == Qt::RightButton)
    {
        mMousePressed = false;
    }

    return false; // Do not consume the event
}

bool Canavar::Engine::Painter::MouseMoved(QMouseEvent *pEvent)
{
    if (mPainterEnabled == false)
    {
        return false; // Do not consume the event if painter is disabled
    }

    mCurrentMousePosition = pEvent->position();

    return false;
}
