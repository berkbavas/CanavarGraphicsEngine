#pragma once

#include "Canavar/Engine/Core/Structs.h"
#include "Canavar/Engine/Node/Object/Object.h"
#include "Canavar/Engine/Primitive/PrimitiveMesh.h"
#include "Canavar/Engine/Util/Macros.h"

#include <QMouseEvent>

namespace Canavar::Engine
{
    class RenderingContext;
    class RenderingManager;
    class CameraManager;
    class GizmoModelRenderer;

    class Gizmo
    {
      public:
        Gizmo(RenderingContext *pRenderingContext, GizmoModelRenderer *pGizmoModelRenderer);
        ~Gizmo() = default;

        void Enter(Object *pObject);
        void Exit();

        bool OnMouseMoved(QMouseEvent *pEvent);
        bool OnMousePressed(QMouseEvent *pEvent);
        bool OnMouseReleased(QMouseEvent *pEvent);

      private:
        enum class GizmoAxis
        {
            None,
            Yaw,
            Pitch,
        };

        GizmoAxis PickAxis(QMouseEvent *pEvent) const;
        QVector3D GetPlaneNormalForAxis(GizmoAxis Axis) const;
        bool IntersectMouseWithGizmoPlane(GizmoAxis Axis, const QPointF &MousePosition, QVector3D &OutIntersectionPoint) const;

        float GetGizmoRadius() const;

        void UpdateTorusTransform();
        void ShowActiveTorus();
        void ShowToruses(bool Show);

        RenderingContext *mRenderingContext{ nullptr };
        RenderingManager *mRendererManager{ nullptr };
        CameraManager *mCameraManager{ nullptr };

        bool mInteracting{ false };

        TorusPtr mYawTorus;   // For visualizing the rotation rings
        TorusPtr mPitchTorus; // For visualizing the rotation rings

        int mYawTorusModelId{ -1 };
        int mPitchTorusModelId{ -1 };

        GizmoAxis mActiveGizmoAxis{ GizmoAxis::None };

        Object *mTargetObject{ nullptr };
        QVector3D mPointOnGizmoPlane;

        float mRotationSpeed{ 1.0f };
    };

    using GizmoPtr = std::shared_ptr<Gizmo>;
}