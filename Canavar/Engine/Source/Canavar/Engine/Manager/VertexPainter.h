#pragma once

#include "Canavar/Engine/Manager/Manager.h"
#include "Canavar/Engine/Manager/RenderingManager/NodeInfo.h"
#include "Canavar/Engine/Manager/RenderingManager/Shader.h"
#include "Canavar/Engine/Node/Object/Scene/Mesh.h"

#include <QMouseEvent>
#include <QOpenGLFunctions_4_3_Core>


namespace Canavar::Engine
{
    class RenderingManager;
    class NodeManager;

    class VertexPainter : public Manager, protected QOpenGLFunctions_4_3_Core
    {
      public:
        explicit VertexPainter(QObject *parent = nullptr);

        void Initialize() override;
        void PostInitialize() override;

        void Update(float ifps);

        void OnMousePressed(QMouseEvent *);
        void OnMouseReleased(QMouseEvent *);
        void OnMouseMoved(QMouseEvent *);
        void OnWheelMoved(QWheelEvent *);

      private:
        RenderingManager *mRendererManager;
        NodeManager *mNodeManager;

        Shader *mVertexPainterShader;

        bool mMousePressed{ false };
        QPointF mCurrentMousePosition;

        NodeInfo mNodeInfo;

        DEFINE_MEMBER(QVector3D, BrushCenter);
        DEFINE_MEMBER(float, BrushRadius, 1.0f);
        DEFINE_MEMBER(QVector4D, BrushColor, QVector4D(1, 0, 0, 1));
        DEFINE_MEMBER_PTR(Mesh, TargetMesh);

        DEFINE_MEMBER(bool, VertexPaintingEnabled, false);
    };

    using VertexPainterPtr = std::shared_ptr<VertexPainter>;
}