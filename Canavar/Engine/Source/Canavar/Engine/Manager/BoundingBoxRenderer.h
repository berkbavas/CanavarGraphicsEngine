#pragma once

#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/Manager/Manager.h"
#include "Canavar/Engine/Node/Object/Camera/Camera.h"
#include "Canavar/Engine/Primitive/CubeStrip.h"

#include <QVector4D>

namespace Canavar::Engine
{
    class NodeManager;
    class CameraManager;
    class ShaderManager;

    class BoundingBoxRenderer : public Manager
    {
      public:
        using Manager::Manager;

        void PostInitialize() override;
        void InRender(PerspectiveCamera* pActiveCamera) override;

      private:
        CubeStrip* mCubeStrip;
        Shader* mLineShader;
        NodeManager* mNodeManager;

        DEFINE_MEMBER(bool, DrawBoundingBoxes, false);
        DEFINE_MEMBER(QVector4D, LineColor, QVector4D(1, 1, 1, 1));
    };
}