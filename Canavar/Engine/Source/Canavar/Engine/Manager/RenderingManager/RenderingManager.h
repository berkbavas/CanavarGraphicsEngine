#pragma once

#include "Canavar/Engine/Core/Constants.h"
#include "Canavar/Engine/Manager/Manager.h"
#include "Canavar/Engine/Manager/RenderingManager/NodeInfo.h"
#include "Canavar/Engine/Manager/RenderingManager/Shader.h"
#include "Canavar/Engine/Node/GlobalNode/Haze/Haze.h"
#include "Canavar/Engine/Node/GlobalNode/Sky/Sky.h"
#include "Canavar/Engine/Node/GlobalNode/Terrain/Terrain.h"
#include "Canavar/Engine/Node/Object/Camera/Camera.h"
#include "Canavar/Engine/Node/Object/Effect/NozzleEffect/NozzleEffect.h"
#include "Canavar/Engine/Node/Object/Light/DirectionalLight.h"
#include "Canavar/Engine/Node/Object/Model/Model.h"

#include <map>

#include <QOpenGLExtraFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>

namespace Canavar::Engine
{
    class ShaderManager;
    class NodeManager;
    class CameraManager;
    class LightManager;
    class BoundingBoxRenderer;

    enum E_Framebuffer
    {
        Default,
        Temp,
        Ping,
        Pong
    };

    class RenderingManager : public Manager, protected QOpenGLExtraFunctions
    {
        Q_OBJECT
      public:
        explicit RenderingManager(QObject *parent = nullptr);

        void Initialize() override;
        void PostInitialize() override;

        void Render(float ifps) override;
        void Resize(int width, int height);

        QVector3D FetchFragmentLocalPositionFromScreen(int x, int y);
        QVector3D FetchFragmentWorldPositionFromScreen(int x, int y);
        NodeInfo FetchNodeInfoFromScreenCoordinates(int x, int y);

      signals:
        void RenderLoop(float ifps);

      private:
        void RenderObjects(float ifps);
        void RenderModel(ModelPtr pModel);
        void RenderNozzleEffect(NozzleEffectPtr pEffect, float ifps);

        void SetUniforms();
        void SetCommonUniforms(Shader *pShader);
        void SetDirectionalLights(Shader *pShader);
        void SetPointLights(Shader *pShader, ObjectPtr pObject);

        void ResizeFramebuffers();

        ShaderManager *mShaderManager;
        NodeManager *mNodeManager;
        CameraManager *mCameraManager;
        LightManager *mLightManager;

        BoundingBoxRenderer *mBoundingBoxRenderer;

        CameraPtr mActiveCamera{ nullptr };

        Shader *mModelShader{ nullptr };
        Shader *mSkyShader{ nullptr };
        Shader *mTerrainShader{ nullptr };
        Shader *mBlurShader{ nullptr };
        Shader *mPostProcessShader{ nullptr };
        Shader *mNozzleEffectShader{ nullptr };
        Shader *mLightningStrikeShader{ nullptr };
        Shader *mLightningStrikeQuadGeneratorShader{ nullptr };

        SkyPtr mSky;
        TerrainPtr mTerrain;
        DirectionalLightPtr mSun;
        HazePtr mHaze;

        Quad *mQuad;

        std::map<E_Framebuffer, QOpenGLFramebufferObject *> mFramebuffers;
        std::map<E_Framebuffer, QOpenGLFramebufferObjectFormat> mFramebufferFormats;

        int mWidth{ INITIAL_WIDTH };
        int mHeight{ INITIAL_HEIGHT };

        DEFINE_MEMBER(int, BlurPass, 4);
        DEFINE_MEMBER(bool, DrawBoundingBoxes, false);

        static constexpr int NUMBER_OF_FBO_ATTACHMENTS = 5;
        static constexpr GLuint FBO_ATTACHMENTS[] = //
            {
                GL_COLOR_ATTACHMENT0, //
                GL_COLOR_ATTACHMENT1, //
                GL_COLOR_ATTACHMENT2, //
                GL_COLOR_ATTACHMENT3, //
                GL_COLOR_ATTACHMENT4  //
            };
    };
};
