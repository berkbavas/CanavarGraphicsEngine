#pragma once

#include "Canavar/Engine/Primitive/PrimitiveMesh.h"
#include "Canavar/Engine/Primitive/TorusData.h"

#include <QList>
#include <QOpenGLExtraFunctions>

namespace Canavar::Engine
{
    class RenderingContext;
    class CameraManager;
    class Shader;
    class NodeManager;

    class GizmoModelRenderer : protected QOpenGLExtraFunctions
    {
      public:
        GizmoModelRenderer(RenderingContext *pRenderingContext);
        ~GizmoModelRenderer() = default;

        void AddModel(const PrimitiveMeshPtr &pModel);

        void Render();

      private:
        RenderingContext *mRenderingContext{ nullptr };
        CameraManager *mCameraManager{ nullptr };
        NodeManager *mNodeManager{ nullptr };

        QList<PrimitiveMeshPtr> mModels;

        Shader *mBasicShader{ nullptr };
        TorusDataPtr mTorusData{ nullptr };
    };

    using GizmoModelRendererPtr = std::shared_ptr<GizmoModelRenderer>;
}