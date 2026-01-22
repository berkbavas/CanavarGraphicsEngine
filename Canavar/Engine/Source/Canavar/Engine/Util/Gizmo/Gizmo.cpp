#include "Gizmo.h"

#include "Canavar/Engine/Core/RenderingContext.h"
#include "Canavar/Engine/Manager/CameraManager.h"
#include "Canavar/Engine/Manager/RenderingManager.h"
#include "Canavar/Engine/Util/Gizmo/GizmoModelRenderer.h"
#include "Canavar/Engine/Util/Math.h"

Canavar::Engine::Gizmo::Gizmo(RenderingContext *pRenderingContext, GizmoModelRenderer *pGizmoModelRenderer)
    : mRenderingContext(pRenderingContext)
    , mRendererManager(pRenderingContext->GetRenderingManager())
    , mCameraManager(pRenderingContext->GetCameraManager())
{
    mYawTorus = std::make_shared<Torus>();
    mPitchTorus = std::make_shared<Torus>();
    mYawTorus->SetVisible(true);
    mPitchTorus->SetVisible(true);

    pGizmoModelRenderer->AddModel(mYawTorus);
    pGizmoModelRenderer->AddModel(mPitchTorus);

    mYawTorusModelId = mYawTorus->GetNodeId();
    mPitchTorusModelId = mPitchTorus->GetNodeId();
}

void Canavar::Engine::Gizmo::Enter(Object *pObject)
{
    mTargetObject = pObject;
    UpdateTorusTransform();
    mYawTorus->SetVisible(true);
    mPitchTorus->SetVisible(true);
}

void Canavar::Engine::Gizmo::Exit()
{
    mTargetObject = nullptr;
    mInteracting = false;
    mActiveGizmoAxis = GizmoAxis::None;
    mYawTorus->SetVisible(false);
    mPitchTorus->SetVisible(false);
}

bool Canavar::Engine::Gizmo::OnMouseMoved(QMouseEvent *pEvent)
{
    if (mTargetObject == nullptr)
    {
        return false;
    }

    if (mInteracting)
    {
        const auto PlaneNormal = GetPlaneNormalForAxis(mActiveGizmoAxis);
        QVector3D CurrentPointOnGizmoPlane;

        if (IntersectMouseWithGizmoPlane(mActiveGizmoAxis, pEvent->position(), CurrentPointOnGizmoPlane))
        {
            QVector3D V0 = mPointOnGizmoPlane - mTargetObject->GetWorldPosition();
            QVector3D V1 = CurrentPointOnGizmoPlane - mTargetObject->GetWorldPosition();

            const float Angle = Math::SignedAngleBetween(V0, V1, PlaneNormal);
            mTargetObject->SetWorldRotation(QQuaternion::fromAxisAndAngle(PlaneNormal, mRotationSpeed * Angle) * mTargetObject->GetWorldRotation());
            mPointOnGizmoPlane = CurrentPointOnGizmoPlane;
            ShowActiveTorus();
        }

        return true;
    }
    else
    {
        mActiveGizmoAxis = PickAxis(pEvent);
        ShowActiveTorus();
    }

    return false;
}

bool Canavar::Engine::Gizmo::OnMousePressed(QMouseEvent *pEvent)
{
    if (mTargetObject == nullptr)
    {
        return false;
    }

    mInteracting = false;
    mActiveGizmoAxis = PickAxis(pEvent);

    if (mActiveGizmoAxis != GizmoAxis::None)
    {
        IntersectMouseWithGizmoPlane(mActiveGizmoAxis, pEvent->position(), mPointOnGizmoPlane);
        mInteracting = true;
    }

    return mInteracting;
}

bool Canavar::Engine::Gizmo::OnMouseReleased(QMouseEvent *pEvent)
{
    mInteracting = false;
    return false;
}

Canavar::Engine::Gizmo::GizmoAxis Canavar::Engine::Gizmo::PickAxis(QMouseEvent *pEvent) const
{
    NodeInfo NodeInfo = mRendererManager->FetchNodeInfoFromScreenCoordinates(static_cast<int>(pEvent->position().x()), static_cast<int>(pEvent->position().y()));

    if (static_cast<int>(NodeInfo.NodeId) == mYawTorusModelId)
    {
        return GizmoAxis::Yaw;
    }
    else if (static_cast<int>(NodeInfo.NodeId) == mPitchTorusModelId)
    {
        return GizmoAxis::Pitch;
    }

    return GizmoAxis::None;
}

QVector3D Canavar::Engine::Gizmo::GetPlaneNormalForAxis(GizmoAxis Axis) const
{
    switch (Axis)
    {
    case GizmoAxis::Yaw:
        return mTargetObject->GetWorldRotation() * POSITIVE_Y;
    case GizmoAxis::Pitch:
        return mTargetObject->GetWorldRotation() * POSITIVE_X;
    default:
        return QVector3D();
    }
}

bool Canavar::Engine::Gizmo::IntersectMouseWithGizmoPlane(GizmoAxis Axis, const QPointF &MousePosition, QVector3D &OutIntersectionPoint) const
{
    const QVector3D PlaneNormal = GetPlaneNormalForAxis(Axis);
    const auto pCamera = mCameraManager->GetActiveCamera();
    const QVector3D RayOrigin = pCamera->GetPosition();
    const QVector3D RayDir = pCamera->ComputeRayFromScreenPosition(MousePosition);
    return Math::IntersectRayPlane(RayOrigin, RayDir, mTargetObject->GetWorldPosition(), PlaneNormal, OutIntersectionPoint);
}

float Canavar::Engine::Gizmo::GetGizmoRadius() const
{
    return 2.0f;
}

void Canavar::Engine::Gizmo::UpdateTorusTransform()
{
    mYawTorus->SetWorldPosition(mTargetObject->GetWorldPosition());
    mPitchTorus->SetWorldPosition(mTargetObject->GetWorldPosition());
    mYawTorus->SetScale(GetGizmoRadius());
    mPitchTorus->SetScale(GetGizmoRadius());

    mYawTorus->SetWorldRotation(mTargetObject->GetWorldRotation() * QQuaternion::fromAxisAndAngle(POSITIVE_X, 90.0f));
    mPitchTorus->SetWorldRotation(mTargetObject->GetWorldRotation() * QQuaternion::fromAxisAndAngle(POSITIVE_Y, 90.0f));
}

void Canavar::Engine::Gizmo::ShowActiveTorus()
{
    if (mTargetObject == nullptr)
    {
        return;
    }

    UpdateTorusTransform();

    mYawTorus->SetColor(QVector3D(1.0f, 0.0f, 0.0f));
    mPitchTorus->SetColor(QVector3D(0.0f, 1.0f, 0.0f));
    mYawTorus->SetVisible(true);
    mPitchTorus->SetVisible(true);

    switch (mActiveGizmoAxis)
    {
    case GizmoAxis::Yaw:
        mYawTorus->SetColor(QVector3D(1.0f, 1.0f, 1.0f));
        break;
    case GizmoAxis::Pitch:
        mPitchTorus->SetColor(QVector3D(1.0f, 1.0f, 1.0f));
        break;
    default:
        break;
    }
}

void Canavar::Engine::Gizmo::ShowToruses(bool Show)
{
    if (mTargetObject == nullptr)
    {
        return;
    }

    UpdateTorusTransform();

    mYawTorus->SetColor(QVector3D(1.0f, 0.0f, 0.0f));
    mPitchTorus->SetColor(QVector3D(0.0f, 1.0f, 0.0f));

    mYawTorus->SetVisible(Show);
    mPitchTorus->SetVisible(Show);
}
