#pragma once

#include "Canavar/Engine/Core/Constants.h"
#include "Canavar/Engine/Manager/Manager.h"
#include "Canavar/Engine/Manager/RenderingManager/NodeInfo.h"
#include "Canavar/Engine/Manager/RenderingManager/Shader.h"
#include "Canavar/Engine/Manager/RenderingManager/ShadowMapping/ShadowMappingRenderer.h"
#include "Canavar/Engine/Node/GlobalNode/Haze/Haze.h"
#include "Canavar/Engine/Node/GlobalNode/Sky/Sky.h"
#include "Canavar/Engine/Node/GlobalNode/Terrain/Terrain.h"
#include "Canavar/Engine/Node/Object/Camera/Camera.h"
#include "Canavar/Engine/Node/Object/Effect/NozzleEffect/NozzleEffect.h"
#include "Canavar/Engine/Node/Object/Light/DirectionalLight.h"
#include "Canavar/Engine/Node/Object/Model/Model.h"

#include <map>

#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLFunctions_4_3_Core>

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
        Pong,
        Bloom
    };

    class RenderingManager : public Manager, protected QOpenGLFunctions_4_3_Core
    {
        Q_OBJECT
      public:
        explicit RenderingManager(QObject *parent = nullptr);

        void Initialize() override;
        void PostInitialize() override;

        void Render(float ifps) override;
        void Resize(int width, int height);

        void OnMousePressed(QMouseEvent *);
        void OnMouseReleased(QMouseEvent *);
        void OnMouseMoved(QMouseEvent *);
        void OnWheelMoved(QWheelEvent *);

        QVector3D FetchFragmentLocalPositionFromScreen(int x, int y);
        QVector3D FetchFragmentWorldPositionFromScreen(int x, int y);
        NodeInfo FetchNodeInfoFromScreenCoordinates(int x, int y);

        QOpenGLFramebufferObject *GetFramebuffer(E_Framebuffer framebuffer) const;

        void RenderToFramebuffer(QOpenGLFramebufferObject *pFramebuffer, Camera *pCamera);

      signals:
        void RenderLoop(float ifps);

      private:
        void RenderObjects(Camera *pCamera, float ifps);
        void RenderModel(ModelPtr pModel);
        void RenderNozzleEffect(NozzleEffectPtr pEffect, Camera *pCamera, float ifps);

        void SetUniforms(Camera *pCamera);
        void SetCommonUniforms(Shader *pShader, Camera *pCamera);
        void SetDirectionalLights(Shader *pShader);
        void SetPointLights(Shader *pShader, Object *pObject);

        void ResizeFramebuffers();
        void ResizeCustomFramebuffers();

        void ApplyPingPong(E_Framebuffer target, int source, int pass);

        ShaderManager *mShaderManager;
        NodeManager *mNodeManager;
        CameraManager *mCameraManager;
        LightManager *mLightManager;

        BoundingBoxRenderer *mBoundingBoxRenderer;
        DEFINE_MEMBER_PTR_CONST(ShadowMappingRenderer, ShadowMappingRenderer);

        Camera *mActiveCamera{ nullptr };

        Shader *mModelShader{ nullptr };
        Shader *mSkyShader{ nullptr };
        Shader *mTerrainShader{ nullptr };
        Shader *mBlurShader{ nullptr };
        Shader *mPostProcessShader{ nullptr };
        Shader *mNozzleEffectShader{ nullptr };
        Shader *mLightningStrikeShader{ nullptr };
        Shader *mLineShader{ nullptr };

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
        DEFINE_MEMBER(bool, ShadowsEnabled, false);
        DEFINE_MEMBER(float, ShadowBias, 0.00005f);
        DEFINE_MEMBER(int, ShadowSamples, 3);

        static constexpr int NUMBER_OF_FBO_ATTACHMENTS = 6;
        static constexpr GLuint FBO_ATTACHMENTS[] = //
            {
                GL_COLOR_ATTACHMENT0, //
                GL_COLOR_ATTACHMENT1, //
                GL_COLOR_ATTACHMENT2, //
                GL_COLOR_ATTACHMENT3, //
                GL_COLOR_ATTACHMENT4, //
            };

        float mIfps;
    };
};
