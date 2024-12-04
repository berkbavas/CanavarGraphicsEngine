#pragma once

#include "Canavar/Engine/Node/Object/Light/DirectionalLight.h"
#include "Canavar/Engine/Util/Logger.h"
#include "Canavar/Engine/Util/Macros.h"

#include <QOpenGLExtraFunctions>

namespace Canavar::Engine
{
    class ShadowMappingFramebuffer;
    class NodeManager;
    class ShaderManager;
    class Shader;

    class ShadowMappingRenderer : protected QOpenGLExtraFunctions
    {
      public:
        ShadowMappingRenderer() = default;

        void Initialize();
        void Render(float ifps);
        GLuint GetShadowMap() const;

      private:
        ShadowMappingFramebuffer *mFramebuffer{ nullptr };

        Shader *mShadowMappingShader{ nullptr };

        QMatrix4x4 mLightProjectionMatrix;
        QMatrix4x4 mLightViewMatrix;

        DEFINE_MEMBER(float, Fov, 60);
        DEFINE_MEMBER(float, ZNear, 10);
        DEFINE_MEMBER(float, ZFar, 1000);
        DEFINE_MEMBER(float, SunDistance, 50);

        DEFINE_MEMBER_PTR(NodeManager, NodeManager);
        DEFINE_MEMBER_PTR(ShaderManager, ShaderManager);
        DEFINE_MEMBER(DirectionalLightPtr, Sun);
        DEFINE_MEMBER_CONST(QMatrix4x4, LightViewProjectionMatrix);
    };
}