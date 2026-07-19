#include "Gizmo.h"

#include "Canavar/Engine/Manager/NodeManager.h"
#include "Canavar/Engine/Manager/Renderer.h"
#include "Canavar/Engine/Model/PrimitiveModel/Primitives.h"
#include "Canavar/Engine/Util/Math.h"

Canavar::Engine::Gizmo::Gizmo(Renderer *pRenderer)
    : mRenderer(pRenderer)
{
    mNodeManager = mRenderer->GetNodeManager();

    // -- Create gizmo primitives for each axis (X, Y, Z)

    // Yaw (Y-axis) gizmo components
    Circle *pYawCircle = mNodeManager->CreateNode<Circle>();
    pYawCircle->SetVisible(false);
    pYawCircle->SetColor(QVector3D(1, 0, 0));
    pYawCircle->SetOverlay(true);

    Disk *pYawDisk = mNodeManager->CreateNode<Disk>();
    pYawDisk->SetVisible(false);
    pYawDisk->SetColor(QVector3D(1, 0, 0));
    pYawDisk->SetOpacity(0.5f);

    Line *pYawLine = mNodeManager->CreateNode<Line>();
    pYawLine->SetVisible(false);
    pYawLine->SetColor(QVector3D(1, 0, 0));

    // Pitch (X-axis) gizmo components
    Circle *pPitchCircle = mNodeManager->CreateNode<Circle>();
    pPitchCircle->SetVisible(false);
    pPitchCircle->SetColor(QVector3D(0, 1, 0));
    pPitchCircle->SetOverlay(true);

    Disk *pPitchDisk = mNodeManager->CreateNode<Disk>();
    pPitchDisk->SetVisible(false);
    pPitchDisk->SetColor(QVector3D(0, 1, 0));
    pPitchDisk->SetOpacity(0.5f);

    Line *pPitchLine = mNodeManager->CreateNode<Line>();
    pPitchLine->SetVisible(false);
    pPitchLine->SetColor(QVector3D(0, 1, 0));

    // Roll (Z-axis) gizmo components
    Circle *pRollCircle = mNodeManager->CreateNode<Circle>();
    pRollCircle->SetVisible(false);
    pRollCircle->SetColor(QVector3D(0, 0, 1));
    pRollCircle->SetOverlay(true);

    Disk *pRollDisk = mNodeManager->CreateNode<Disk>();
    pRollDisk->SetVisible(false);
    pRollDisk->SetColor(QVector3D(0, 0, 1));
    pRollDisk->SetOpacity(0.5f);

    Line *pRollLine = mNodeManager->CreateNode<Line>();
    pRollLine->SetVisible(false);
    pRollLine->SetColor(QVector3D(0, 0, 1));

    // Interaction line for visual feedback during dragging
    mInteractionLine = mNodeManager->CreateNode<Line>();
    mInteractionLine->SetVisible(false);
    mInteractionLine->SetColor(QVector3D(1, 1, 1));
    mInteractionLine->SetOverlay(true);

    mAxisCircles[Axis::X] = pPitchCircle;
    mAxisDisks[Axis::X] = pPitchDisk;
    mAxisLines[Axis::X] = pPitchLine;

    mAxisCircles[Axis::Y] = pYawCircle;
    mAxisDisks[Axis::Y] = pYawDisk;
    mAxisLines[Axis::Y] = pYawLine;

    mAxisCircles[Axis::Z] = pRollCircle;
    mAxisDisks[Axis::Z] = pRollDisk;
    mAxisLines[Axis::Z] = pRollLine;

    mGizmoPrimitives.push_back(pYawCircle);
    mGizmoPrimitives.push_back(pYawDisk);
    mGizmoPrimitives.push_back(pPitchCircle);
    mGizmoPrimitives.push_back(pPitchDisk);
    mGizmoPrimitives.push_back(pYawLine);
    mGizmoPrimitives.push_back(pPitchLine);
    mGizmoPrimitives.push_back(pRollCircle);
    mGizmoPrimitives.push_back(pRollDisk);
    mGizmoPrimitives.push_back(pRollLine);
    mGizmoPrimitives.push_back(mInteractionLine);
}

void Canavar::Engine::Gizmo::Enter(Object *pTargetObject)
{
    mTargetObject = pTargetObject;
    mIsActive = true;
    mActiveAxis = Axis::None;

    UpdateGizmoSphereRadius();
    UpdateGizmoVisuals();
}

void Canavar::Engine::Gizmo::Exit()
{
    mTargetObject = nullptr;
    mIsActive = false;
    mActiveAxis = Axis::None;

    // Hide all gizmo primitives when exiting
    for (auto *pPrimitive : mGizmoPrimitives)
    {
        pPrimitive->SetVisible(false);
    }
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

void Canavar::Engine::Gizmo::SetRotation(const QQuaternion &Rotation)
{
    if (mTargetObject)
    {
        mTargetObject->SetRotation(Rotation);
    }
}

QVector3D Canavar::Engine::Gizmo::GetPosition() const
{
    return mTargetObject ? mTargetObject->GetWorldPosition() : QVector3D();
}

QQuaternion Canavar::Engine::Gizmo::GetRotation() const
{
    return mTargetObject ? mTargetObject->GetWorldRotation() : QQuaternion();
}

void Canavar::Engine::Gizmo::UpdateGizmoVisuals()
{
    UpdateGizmoVisualsTransformations();
    UpdateGizmoVisualsAppearance();
}

void Canavar::Engine::Gizmo::UpdateGizmoVisualsTransformations()
{
    const auto Position = GetPosition();
    const auto Rotation = GetRotation();

    for (const auto &[AxisType, pCircle] : mAxisCircles)
    {
        pCircle->SetPosition(Position);
        pCircle->SetRotation(GetAxisCircleRotation(AxisType));
        pCircle->SetScale(mSphereRadius);
    }

    for (const auto &[AxisType, pDisk] : mAxisDisks)
    {
        pDisk->SetPosition(Position);
        pDisk->SetRotation(GetAxisCircleRotation(AxisType));
        pDisk->SetScale(mSphereRadius);
    }

    for (const auto &[AxisType, pLine] : mAxisLines)
    {
        pLine->SetPoints(Position, Position + GetAxisCircleNormal(AxisType) * mSphereRadius);
    }

    mInteractionLine->SetPoints(Position, mPreviousIntersectionPoint);
}

void Canavar::Engine::Gizmo::UpdateGizmoVisualsAppearance()
{
    if (mIsDragging)
    {
        for (const auto &[AxisType, pDisk] : mAxisDisks)
        {
            pDisk->SetVisible(AxisType == mActiveAxis); // Show only the disk corresponding to the active axis
            pDisk->SetOverlay(true);
        }

        for (const auto &[AxisType, pCircle] : mAxisCircles)
        {
            pCircle->SetVisible(AxisType == mActiveAxis); // Show only the circle corresponding to the active axis
            pCircle->SetOpacity(1.0f);
            pCircle->SetThickness(4.0f);
        }

        for (const auto &[AxisType, pLine] : mAxisLines)
        {
            pLine->SetVisible(AxisType == mActiveAxis); // Show only the line corresponding to the active axis
            pLine->SetOverlay(true);
        }

        mInteractionLine->SetVisible(true);
    }
    else
    {
        // Always hide all lines when not dragging
        for (const auto &[AxisType, pLineModel] : mAxisLines)
        {
            pLineModel->SetVisible(false);
        }

        if (mActiveAxis == Axis::None)
        {
            // Default state: show all circles with lower opacity and hide disks
            for (const auto &[AxisType, pCircleModel] : mAxisCircles)
            {
                pCircleModel->SetVisible(true);
                pCircleModel->SetOpacity(0.5f);
                pCircleModel->SetThickness(3.0f);
            }

            // Hide disks
            for (const auto &[AxisType, pDiskModel] : mAxisDisks)
            {
                pDiskModel->SetVisible(false);
            }
        }
        else
        {
            // Highlight the active axis circle and disk
            for (const auto &[AxisType, pCircleModel] : mAxisCircles)
            {
                pCircleModel->SetVisible(true);
                pCircleModel->SetOpacity(AxisType == mActiveAxis ? 1.0f : 0.5f);
                pCircleModel->SetThickness(AxisType == mActiveAxis ? 4.0f : 3.0f);
            }

            for (const auto &[AxisType, pDiskModel] : mAxisDisks)
            {
                pDiskModel->SetVisible(AxisType == mActiveAxis);
                pDiskModel->SetOverlay(false);
            }
        }

        mInteractionLine->SetVisible(false);
    }
}

bool Canavar::Engine::Gizmo::CalculateMouseRaySphereIntersection(const QPointF &MousePos, QVector3D &OutIntersectionPoint) const
{
    const auto pActiveCamera = mRenderer->GetActiveCamera();
    const auto RayDir = pActiveCamera->ComputeRayFromScreen(MousePos);
    const auto RayOrigin = pActiveCamera->GetWorldPosition();
    const auto GizmoPos = GetPosition();
    const auto Success = Math::IntersectRaySphere(RayOrigin, RayDir, GizmoPos, mSphereRadius, OutIntersectionPoint);

    return Success;
}

bool Canavar::Engine::Gizmo::CalculateMouseRayViewPlaneIntersection(const QPointF &MousePos, QVector3D &OutIntersectionPoint) const
{
    const auto pActiveCamera = mRenderer->GetActiveCamera();
    const auto RayDir = pActiveCamera->ComputeRayFromScreen(MousePos);
    const auto RayOrigin = pActiveCamera->GetWorldPosition();
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
    LocalAxisDirections[Axis::Z] = GetLocalAxisDirection(Axis::Z);

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
    case Axis::Z:
        return Rotation * POSITIVE_Z;
    case Axis::None:
    default:
        return QVector3D(0.0f, 0.0f, 0.0f); // No direction for None axis
    }
}

QVector3D Canavar::Engine::Gizmo::GetRayDirectionFromMousePosition(const QPointF &MousePos) const
{
    return mRenderer->GetActiveCamera()->ComputeRayFromScreen(MousePos);
}

QVector3D Canavar::Engine::Gizmo::GetAxisCircleNormal(Axis Axis) const
{
    switch (Axis)
    {
    case Axis::X:
        return GetLocalAxisDirection(Axis::Y); // The normal of the X-axis circle is along the Y-axis
    case Axis::Y:
        return GetLocalAxisDirection(Axis::X); // The normal of the Y-axis circle is along the X-axis
    case Axis::Z:
        return GetLocalAxisDirection(Axis::X); // The normal of the Z-axis circle is along the X-axis (?)
    case Axis::None:
    default:
        return QVector3D(0.0f, 0.0f, 0.0f); // No normal for None axis
    }
}

QQuaternion Canavar::Engine::Gizmo::GetAxisCircleRotation(Axis Axis) const
{
    const auto Rotation = GetRotation();

    switch (Axis)
    {
    case Axis::X:
        return Rotation * QQuaternion::fromAxisAndAngle(POSITIVE_Y, 90.0f);
    case Axis::Y:
        return Rotation * QQuaternion::fromAxisAndAngle(POSITIVE_X, 90.0f);
    case Axis::Z:
        return Rotation; // Z-axis circle aligns with the object's rotation
    case Axis::None:
    default:
        return Rotation;
    }
}

void Canavar::Engine::Gizmo::UpdateGizmoSphereRadius()
{
    if (mTargetObject)
    {
        // Update the gizmo sphere radius based on the target object's bounding sphere radius
        mSphereRadius = std::max(1.0f, mTargetObject->GetBoundingSphereRadius());
    }
    else
    {
        // Default radius if no target object is set
        mSphereRadius = 1.0f;
    }
}
