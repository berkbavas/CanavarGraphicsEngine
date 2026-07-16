#include "Gizmo.h"

#include "Canavar/Engine/Manager/NodeManager.h"
#include "Canavar/Engine/Manager/Renderer.h"
#include "Canavar/Engine/Model/PrimitiveModel/Primitives.h"
#include "Canavar/Engine/Util/Math.h"

Canavar::Engine::Gizmo::Gizmo(Renderer *pRenderer)
    : mRenderer(pRenderer)
{
    mNodeManager = mRenderer->GetNodeManager();

    mYawCircle = mNodeManager->CreateNode<Circle>();
    mYawCircle->SetScale(mSphereRadius);
    mYawCircle->SetVisible(false);
    mYawCircle->SetColor(QVector3D(1, 0, 0));
    mYawCircle->SetOverlay(true);

    mYawDisk = mNodeManager->CreateNode<Disk>();
    mYawDisk->SetScale(mSphereRadius);
    mYawDisk->SetVisible(false);
    mYawDisk->SetColor(QVector3D(1, 0, 0));
    mYawDisk->SetOpacity(0.25f);
    mYawDisk->SetOverlay(true);

    mPitchCircle = mNodeManager->CreateNode<Circle>();
    mPitchCircle->SetScale(mSphereRadius);
    mPitchCircle->SetVisible(false);
    mPitchCircle->SetColor(QVector3D(0, 1, 0));
    mPitchCircle->SetOverlay(true);

    mPitchDisk = mNodeManager->CreateNode<Disk>();
    mPitchDisk->SetScale(mSphereRadius);
    mPitchDisk->SetVisible(false);
    mPitchDisk->SetColor(QVector3D(0, 1, 0));
    mPitchDisk->SetOpacity(0.25f);
    mPitchDisk->SetOverlay(true);

    mInteractionLine = mNodeManager->CreateNode<Line>();
    mInteractionLine->SetVisible(false);
    mInteractionLine->SetColor(QVector3D(1, 1, 1));
    mInteractionLine->SetOverlay(true);

    mYawLine = mNodeManager->CreateNode<Line>();
    mYawLine->SetVisible(false);
    mYawLine->SetColor(QVector3D(1, 0, 0));
    mYawLine->SetOverlay(true);

    mPitchLine = mNodeManager->CreateNode<Line>();
    mPitchLine->SetVisible(false);
    mPitchLine->SetColor(QVector3D(0, 1, 0));
    mPitchLine->SetOverlay(true);
}

void Canavar::Engine::Gizmo::Enter(Object *pTargetObject)
{
    mTargetObject = pTargetObject;
    mIsActive = true;

    SetPosition(mTargetObject->GetPosition());
    SetRotation(mTargetObject->GetRotation());
}

void Canavar::Engine::Gizmo::Exit()
{
    mTargetObject = nullptr;
    mIsActive = false;
}

bool Canavar::Engine::Gizmo::OnMousePressed(QMouseEvent *pEvent)
{
    if (!mIsActive || !mTargetObject)
    {
        return false;
    }

    bool Handled = false;

    if (pEvent->button() == Qt::LeftButton)
    {
        const auto CurrentMousePosition = pEvent->position();
        mActiveAxis = DetermineActiveAxis(CurrentMousePosition);
        if (mActiveAxis != Axis::None)
        {
            if (CalculateMouseRayViewPlaneIntersection(CurrentMousePosition, mPreviousIntersectionPoint))
            {
                mIsDragging = true;
                Handled = true; // Indicate that the event was handled
            }
        }

        UpdateGizmoVisuals();
    }

    return Handled; // Return whether the event was handled
}

bool Canavar::Engine::Gizmo::OnMouseReleased(QMouseEvent *pEvent)
{
    bool Handled = false;

    if (pEvent->button() == Qt::LeftButton && mIsDragging)
    {
        mIsDragging = false;
        mActiveAxis = Axis::None;
        UpdateGizmoVisuals();
        Handled = true; // Indicate that the event was handled
    }

    return Handled; // Return whether the event was handled
}

bool Canavar::Engine::Gizmo::OnMouseMoved(QMouseEvent *pEvent)
{
    if (!mIsActive || !mTargetObject)
    {
        return false;
    }

    const auto CurrentMousePos = pEvent->position();
    bool Handled = false;

    if (mIsDragging)
    {
        QVector3D CurrentIntersectionPoint;
        if (CalculateMouseRayViewPlaneIntersection(CurrentMousePos, CurrentIntersectionPoint))
        {
            const auto pActiveCamera = mRenderer->GetActiveCamera();
            const auto ViewPlaneNormal = -pActiveCamera->GetViewDirection();
            const auto Position = GetPosition();
            const auto V0 = mPreviousIntersectionPoint - Position;
            const auto V1 = CurrentIntersectionPoint - Position;
            const auto AxisDirection = GetLocalAxisDirection(mActiveAxis);
            auto Angle = Math::SignedAngleBetween(V1.normalized(), V0.normalized(), ViewPlaneNormal);

            if (QVector3D::dotProduct(AxisDirection, ViewPlaneNormal) > 0)
            {
                Angle = -Angle;
            }

            const auto RotationDelta = QQuaternion::fromAxisAndAngle(AxisDirection, Angle);

            SetRotation(RotationDelta * GetRotation());
            mPreviousIntersectionPoint = CurrentIntersectionPoint;
            Handled = true; // Indicate that the event was handled
        }
    }
    else
    {
        // If not interacting, check if the mouse is over the gizmo for potential interaction
        mActiveAxis = DetermineActiveAxis(CurrentMousePos);
        if (mActiveAxis != Axis::None)
        {
            Handled = true; // Indicate that the event was handled
        }
    }

    UpdateGizmoVisuals();

    return Handled; // Return whether the event was handled
}

void Canavar::Engine::Gizmo::SetPosition(const QVector3D &Position)
{
    if (mTargetObject)
    {
        mTargetObject->SetPosition(Position);
    }
}

void Canavar::Engine::Gizmo::SetScale(float Scale)
{
    if (mTargetObject)
    {
        mTargetObject->SetScale(Scale);
    }
}

void Canavar::Engine::Gizmo::SetRotation(const QQuaternion &Rotation)
{
    if (mTargetObject)
    {
        mTargetObject->SetRotation(Rotation);
    }
}

QVector3D Canavar::Engine::Gizmo::GetPosition() const
{
    return mTargetObject ? mTargetObject->GetPosition() : QVector3D();
}

QQuaternion Canavar::Engine::Gizmo::GetRotation() const
{
    return mTargetObject ? mTargetObject->GetRotation() : QQuaternion();
}

void Canavar::Engine::Gizmo::UpdateGizmoVisuals()
{
    const auto Position = GetPosition();
    const auto Rotation = GetRotation();

    mYawCircle->SetPosition(Position);
    mYawCircle->SetRotation(Rotation * QQuaternion::fromAxisAndAngle(POSITIVE_X, 90));
    mYawDisk->SetPosition(Position);
    mYawDisk->SetRotation(Rotation * QQuaternion::fromAxisAndAngle(POSITIVE_X, 90));

    mPitchCircle->SetPosition(Position);
    mPitchCircle->SetRotation(Rotation * QQuaternion::fromAxisAndAngle(POSITIVE_Y, 90));
    mPitchDisk->SetPosition(Position);
    mPitchDisk->SetRotation(Rotation * QQuaternion::fromAxisAndAngle(POSITIVE_Y, 90));

    mYawLine->SetPoints(Position, Position + GetLocalAxisDirection(Axis::X) * mSphereRadius);
    mPitchLine->SetPoints(Position, Position + GetLocalAxisDirection(Axis::Y) * mSphereRadius);

    mInteractionLine->SetVisible(mIsDragging);
    mInteractionLine->SetPoints(Position, mPreviousIntersectionPoint);

    if (mIsDragging)
    {
        switch (mActiveAxis)
        {
        case Axis::X:
            mPitchCircle->SetThickness(3.0f);
            mPitchCircle->SetOpacity(1.0f);
            mPitchCircle->SetVisible(true);
            mPitchDisk->SetVisible(true);
            mYawCircle->SetVisible(false);
            mYawDisk->SetVisible(false);
            mYawLine->SetVisible(false);
            mPitchLine->SetVisible(true);
            break;
        case Axis::Y:
            mYawCircle->SetThickness(3.0f);
            mYawCircle->SetOpacity(1.0f);
            mYawCircle->SetVisible(true);
            mYawDisk->SetVisible(true);
            mPitchCircle->SetVisible(false);
            mPitchDisk->SetVisible(false);
            mYawLine->SetVisible(true);
            mPitchLine->SetVisible(false);
            break;
        case Axis::None:
            break; // Should not happen, but we can safely ignore it
        }
    }
    else
    {
        // Hide the disks when not interacting
        mYawDisk->SetVisible(false);
        mPitchDisk->SetVisible(false);
        mYawLine->SetVisible(false);
        mPitchLine->SetVisible(false);

        switch (mActiveAxis)
        {
        case Axis::X:
            mPitchCircle->SetThickness(3.0f);
            mPitchCircle->SetOpacity(1.0f);
            mPitchCircle->SetVisible(true);
            mYawCircle->SetVisible(true);
            mYawCircle->SetOpacity(0.5f);
            break;
        case Axis::Y:
            mYawCircle->SetThickness(3.0f);
            mYawCircle->SetOpacity(1.0f);
            mYawCircle->SetVisible(true);
            mPitchCircle->SetVisible(true);
            mPitchCircle->SetOpacity(0.5f);
            break;
        case Axis::None:
            mPitchCircle->SetVisible(true);
            mPitchCircle->SetOpacity(0.5f);
            mYawCircle->SetVisible(true);
            mYawCircle->SetOpacity(0.5f);
            break;
        }
    }
}

bool Canavar::Engine::Gizmo::CalculateMouseRaySphereIntersection(const QPointF &MousePos, QVector3D &OutIntersectionPoint) const
{
    const auto pActiveCamera = mRenderer->GetActiveCamera();
    const auto RayDir = pActiveCamera->ComputeRayFromScreen(MousePos);
    const auto RayOrigin = pActiveCamera->GetPosition();
    const auto GizmoPos = GetPosition();
    const auto Success = Math::IntersectRaySphere(RayOrigin, RayDir, GizmoPos, mSphereRadius, OutIntersectionPoint);

    return Success;
}

bool Canavar::Engine::Gizmo::CalculateMouseRayViewPlaneIntersection(const QPointF &MousePos, QVector3D &OutIntersectionPoint) const
{
    const auto pActiveCamera = mRenderer->GetActiveCamera();
    const auto RayDir = pActiveCamera->ComputeRayFromScreen(MousePos);
    const auto RayOrigin = pActiveCamera->GetPosition();
    const auto GizmoPos = GetPosition();
    const auto ViewPlaneNormal = -pActiveCamera->GetViewDirection();
    const auto Success = Math::IntersectRayPlane(RayOrigin, RayDir, GizmoPos, ViewPlaneNormal, OutIntersectionPoint);

    return Success;
}

Canavar::Engine::Gizmo::Axis Canavar::Engine::Gizmo::DetermineActiveAxis(const QVector3D &IntersectionPoint) const
{
    const auto GizmoPos = GetPosition();
    const auto GizmoToIntersection = IntersectionPoint - GizmoPos;
    const auto GizmoToIntersectionNormalized = GizmoToIntersection.normalized();
    float MaxScore = mAxisSelectionThreshold;
    Axis BestAxis = Axis::None;

    std::map<Axis, QVector3D> LocalAxisDirections;
    LocalAxisDirections[Axis::None] = QVector3D(0.0f, 0.0f, 0.0f); // Placeholder for no active axis
    LocalAxisDirections[Axis::X] = GetLocalAxisDirection(Axis::X); // X-axis direction (Pitch axis)
    LocalAxisDirections[Axis::Y] = GetLocalAxisDirection(Axis::Y);

    for (const auto &[AxisType, LocalAxisDirection] : LocalAxisDirections)
    {
        if (AxisType == Axis::None)
        {
            continue; // Skip the "None" axis
        }

        const auto Dot = QVector3D::dotProduct(GizmoToIntersectionNormalized, LocalAxisDirection.normalized());
        const auto Score = 1.0f - std::abs(Dot);
        if (Score > MaxScore)
        {
            MaxScore = Score;
            BestAxis = AxisType;
        }
    }
    return BestAxis;
}

Canavar::Engine::Gizmo::Axis Canavar::Engine::Gizmo::DetermineActiveAxis(const QPointF &MousePos) const
{
    QVector3D IntersectionPoint;
    const auto Success = CalculateMouseRaySphereIntersection(MousePos, IntersectionPoint);

    if (Success)
    {
        return DetermineActiveAxis(IntersectionPoint);
    }

    return Axis::None;
}

QVector3D Canavar::Engine::Gizmo::GetLocalAxisDirection(Axis AxisType) const
{
    const auto Rotation = GetRotation();
    switch (AxisType)
    {
    case Axis::X:
        return Rotation * POSITIVE_X;
    case Axis::Y:
        return Rotation * POSITIVE_Y;
    case Axis::None:
    default:
        return QVector3D(0.0f, 0.0f, 0.0f); // No direction for None axis
    }
}

QVector3D Canavar::Engine::Gizmo::GetRayDirectionFromMousePosition(const QPointF &MousePos) const
{
    return mRenderer->GetActiveCamera()->ComputeRayFromScreen(MousePos);
}
