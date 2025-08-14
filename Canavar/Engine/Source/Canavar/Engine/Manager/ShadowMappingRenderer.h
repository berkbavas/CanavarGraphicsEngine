#pragma once

#include "Canavar/Engine/Manager/Manager.h"
#include "Canavar/Engine/Node/Object/Light/DirectionalLight.h"
#include "Canavar/Engine/Util/Logger.h"
#include "Canavar/Engine/Util/Macros.h"

#include <QOpenGLFunctions>

namespace Canavar::Engine
{
    class ShadowMappingFramebuffer;
    class NodeManager;
    class ShaderManager;
    class CameraManager;
    class Shader;
    class Model;

    class ShadowMappingRenderer : public Manager
    {
      public:
        using Manager::Manager;

        void Initialize() override;
        void PostInitialize() override;
        void Render(PerspectiveCamera *pActiveCamera) override;

        GLuint GetShadowMapTexture() const;

      private:
        void RenderForShadowMapping();
        void CalculateShadowViewProjectionMatrix();

        NodeManager *mNodeManager{ nullptr };

        ShadowMappingFramebuffer *mFramebuffer{ nullptr };

        Shader *mShadowMappingShader{ nullptr };

        QMatrix4x4 mLightProjectionMatrix;
        QMatrix4x4 mLightViewMatrix;

        DEFINE_MEMBER(bool, ShadowsEnabled, false);
        DEFINE_MEMBER(float, ShadowBias, 0.00005f);
        DEFINE_MEMBER(int, ShadowSamples, 1);
        DEFINE_MEMBER(float, Fov, 60);
        DEFINE_MEMBER(float, ZNear, 10);
        DEFINE_MEMBER(float, ZFar, 1000);
        DEFINE_MEMBER(float, SunDistance, 50);
        DEFINE_MEMBER(float, OrthographicSize, 200);
        DEFINE_MEMBER(bool, UseOrthographicProjection, false);
        DEFINE_MEMBER(DirectionalLightPtr, Sun);

        DEFINE_MEMBER_CONST(QMatrix4x4, LightViewProjectionMatrix);
    };
}