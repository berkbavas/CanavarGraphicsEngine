#pragma once

#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/Core/Structs.h"
#include "Canavar/Engine/Manager/Manager.h"
#include "Canavar/Engine/Node/Object/Scene/Mesh.h"

#include <QMouseEvent>

namespace Canavar::Engine
{
    class RenderingManager;
    class NodeManager;

    class Painter : public Manager
    {
      public:
        using Manager::Manager;

        void PostInitialize() override;

        void Update(float ifps);

        bool MousePressed(QMouseEvent *) override;
        bool MouseReleased(QMouseEvent *) override;
        bool MouseMoved(QMouseEvent *) override;

      private:
        RenderingManager *mRendererManager;
        NodeManager *mNodeManager;

        bool mMousePressed{ false };
        QPointF mCurrentMousePosition;

        NodeInfo mNodeInfo;

        bool mPaint{ false };

        DEFINE_MEMBER(QVector3D, BrushColor, QVector3D(1, 1, 0));
        DEFINE_MEMBER_PTR(Mesh, TargetMesh);

        DEFINE_MEMBER(bool, PainterEnabled, false);
    };

    using PainterPtr = std::shared_ptr<Painter>;
}