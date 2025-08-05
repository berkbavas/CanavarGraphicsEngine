#pragma once

#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/Node/Object/Camera/Camera.h"
#include "Canavar/Engine/Primitive/CubeStrip.h"

#include <QVector4D>

namespace Canavar::Engine
{
    class NodeManager;
    class CameraManager;
    class ShaderManager;

    class BoundingBoxRenderer
    {
      public:
        void Initialize();
        void Render(Camera* pCamera, float ifps);

      private:
        Shader* mLineShader;

        CubeStrip* mCubeStrip;

        DEFINE_MEMBER_PTR(NodeManager, NodeManager);
        DEFINE_MEMBER_PTR(ShaderManager, ShaderManager);

        static constexpr QVector4D LINE_COLOR = QVector4D(1, 1, 1, 1);
    };
}