#pragma once

#include "Canavar/Engine/Core/EventReceiver.h"

#include <memory>

#include <QQuaternion>
#include <QVector3D>

namespace Canavar::Engine
{
    class Renderer;
    class NodeManager;
    class Object;
    class Line;
    class Circle;
    class Disk;

    class Gizmo : public EventReceiver
    {
      public:
        explicit Gizmo(Renderer *pRenderer);
        virtual ~Gizmo() = default;

        void Enter(Object *pTargetObject);
        void Exit();

        bool OnMousePressed(QMouseEvent *pEvent) override;
        bool OnMouseReleased(QMouseEvent *pEvent) override;
        bool OnMouseMoved(QMouseEvent *pEvent) override;

        void SetPosition(const QVector3D &Position);
        void SetScale(float Scale);
        void SetRotation(const QQuaternion &Rotation);

        QVector3D GetPosition() const;
        QQuaternion GetRotation() const;

      private:
        enum class Axis
        {
            None,
            X,
            Y
        };

        void UpdateGizmoVisuals();
        bool CalculateMouseRaySphereIntersection(const QPointF &MousePos, QVector3D &OutIntersectionPoint) const;
        bool CalculateMouseRayViewPlaneIntersection(const QPointF &MousePos, QVector3D &OutIntersectionPoint) const;
        Axis DetermineActiveAxis(const QVector3D &IntersectionPoint) const;
        Axis DetermineActiveAxis(const QPointF &MousePos) const;
        QVector3D GetLocalAxisDirection(Axis AxisType) const;
        QVector3D GetRayDirectionFromMousePosition(const QPointF &MousePos) const;

        // Gizmo state
        bool mIsActive{ false };
        bool mIsDragging{ false };
        Axis mActiveAxis{ Axis::None };
        QVector3D mPreviousIntersectionPoint{ 0, 0, 0 };

        // Traits of the gizmo
        float mSphereRadius{ 10.0f };           // Radius of the gizmo's interaction sphere
        float mAxisSelectionThreshold{ 0.85f }; // Threshold for selecting an axis based on the angle between the ray and the axis direction

        Object *mTargetObject{ nullptr };

        // Gizmo visual components
        Circle *mYawCircle{ nullptr };
        Circle *mPitchCircle{ nullptr };
        Disk *mYawDisk{ nullptr };
        Disk *mPitchDisk{ nullptr };
        Line *mYawLine{ nullptr };
        Line *mPitchLine{ nullptr };
        Line *mInteractionLine{ nullptr };

        // Global shortcuts
        Renderer *mRenderer{ nullptr };
        NodeManager *mNodeManager{ nullptr };
    };

    using GizmoPtr = std::unique_ptr<Gizmo>;

}