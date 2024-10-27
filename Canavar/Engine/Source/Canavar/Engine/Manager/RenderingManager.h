#pragma once

#include "Canavar/Engine/Core/Constants.h"
#include "Canavar/Engine/Manager/Manager.h"
#include "Canavar/Engine/Node/Camera/Camera.h"
#include "Canavar/Engine/Node/Effects/NozzleEffect/NozzleEffect.h"
#include "Canavar/Engine/Node/Haze/Haze.h"
#include "Canavar/Engine/Node/Light/DirectionalLight.h"
#include "Canavar/Engine/Node/Model/Model.h"
#include "Canavar/Engine/Node/Sky/Sky.h"
#include "Canavar/Engine/Node/Terrain/Terrain.h"
#include "Canavar/Engine/Util/Shader.h"

#include <map>
#include <memory>

#include <QOpenGLExtraFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>

namespace Canavar::Engine
{
    class ShaderManager;
    class NodeManager;
    class CameraManager;
    class LightManager;

    enum E_Framebuffer
    {
        Default,
        Temp,
        Ping,
        Pong
    };

    class RenderingManager : public Manager, protected QOpenGLExtraFunctions
    {
      public:
        explicit RenderingManager(QObject *parent = nullptr);

        void Initialize() override;
        void PostInitialize() override;

        void Render(float ifps);
        void Resize(int width, int height);

      private:
        void RenderModel(ModelPtr pModel);
        void RenderNozzleEffect(NozzleEffectPtr pEffect, float ifps);

        void SetUniforms();
        void SetCommonUniforms(Shader *pShader);
        void SetDirectionalLights(Shader *pShader);
        void SetPointLights(Shader *pShader, NodePtr pNode);

        void ResizeFramebuffers();

        ShaderManager *mShaderManager;
        NodeManager *mNodeManager;
        CameraManager *mCameraManager;
        LightManager *mLightManager;

        CameraPtr mActiveCamera{ nullptr };

        Shader *mModelShader{ nullptr };
        Shader *mSkyShader{ nullptr };
        Shader *mTerrainShader{ nullptr };
        Shader *mBlurShader{ nullptr };
        Shader *mPostProcessShader{ nullptr };
        Shader *mNozzleEffectShader{ nullptr };

        SkyPtr mSky;
        TerrainPtr mTerrain;
        DirectionalLightPtr mSun;
        HazePtr mHaze;

        Quad *mQuad;

        std::map<E_Framebuffer, QOpenGLFramebufferObject *> mFramebuffers;
        std::map<E_Framebuffer, QOpenGLFramebufferObjectFormat> mFramebufferFormats;

        int mWidth{ INITIAL_WIDTH };
        int mHeight{ INITIAL_HEIGHT };

        int mBlurPass{ 4 };
    };
};
