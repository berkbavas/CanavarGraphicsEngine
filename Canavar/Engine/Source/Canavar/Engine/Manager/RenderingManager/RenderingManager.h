#pragma once

#include "Canavar/Engine/Core/Constants.h"
#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/Core/Structs.h"
#include "Canavar/Engine/Manager/Manager.h"
#include "Canavar/Engine/Manager/RenderingManager/CrossSectionAnalyzer/CrossSectionAnalyzer.h"
#include "Canavar/Engine/Manager/RenderingManager/ShadowMapping/ShadowMappingRenderer.h"
#include "Canavar/Engine/Node/Global/Haze/Haze.h"
#include "Canavar/Engine/Node/Global/Sky/Sky.h"
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

    enum Framebuffer
    {
        Multisample,
        Singlesample,
        Temp1,
        Temp2
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

        QVector3D FetchFragmentLocalPositionFromScreen(int x, int y);
        QVector3D FetchFragmentWorldPositionFromScreen(int x, int y);
        NodeInfo FetchNodeInfoFromScreenCoordinates(int x, int y);

        QOpenGLFramebufferObject *GetFramebuffer(Framebuffer framebuffer) const;

        void RenderToFramebuffer(QOpenGLFramebufferObject *pFramebuffer, Camera *pCamera);

      signals:
        void RenderLoop(float ifps);

      private:
        void RenderObjects(Camera *pCamera, float ifps);
        void RenderModel(ModelPtr pModel, RenderPass renderPass);
        void RenderNozzleEffect(NozzleEffectPtr pEffect, Camera *pCamera, float ifps);

        void SetUniforms(Camera *pCamera);
        void SetCommonUniforms(Shader *pShader, Camera *pCamera);
        void SetDirectionalLights(Shader *pShader);
        void SetPointLights(Shader *pShader, Object *pObject);

        void ResizeFramebuffers();
        void ResizeCustomFramebuffers();

        void ApplyPingPong(Framebuffer target, int source, int pass);

        ShaderManager *mShaderManager;
        NodeManager *mNodeManager;
        CameraManager *mCameraManager;
        LightManager *mLightManager;

        BoundingBoxRenderer *mBoundingBoxRenderer;
        DEFINE_MEMBER_PTR_CONST(ShadowMappingRenderer, ShadowMappingRenderer);

        Camera *mActiveCamera{ nullptr };

        Shader *mModelShader{ nullptr };
        Shader *mSkyShader{ nullptr };
        Shader *mBlurShader{ nullptr };
        Shader *mPostProcessShader{ nullptr };
        Shader *mNozzleEffectShader{ nullptr };
        Shader *mLightningStrikeShader{ nullptr };
        Shader *mLineShader{ nullptr };

        SkyPtr mSky;
        DirectionalLightPtr mSun;
        HazePtr mHaze;

        Quad *mQuad;

        std::map<Framebuffer, QOpenGLFramebufferObject *> mFramebuffers;
        std::map<Framebuffer, QOpenGLFramebufferObjectFormat> mFramebufferFormats;

        int mWidth{ INITIAL_WIDTH };
        int mHeight{ INITIAL_HEIGHT };

        DEFINE_MEMBER(bool, DrawBoundingBoxes, false);
        DEFINE_MEMBER(bool, ShadowsEnabled, false);
        DEFINE_MEMBER(float, ShadowBias, 0.00005f);
        DEFINE_MEMBER(int, ShadowSamples, 1);

        static constexpr int NUMBER_OF_FBO_ATTACHMENTS = 5;
        static constexpr GLuint FBO_ATTACHMENTS[] = //
            {
                GL_COLOR_ATTACHMENT0, // Color
                GL_COLOR_ATTACHMENT1, // Fragment local position
                GL_COLOR_ATTACHMENT2, // Fragment world position
                GL_COLOR_ATTACHMENT3, // Node info
                GL_COLOR_ATTACHMENT4, // Distance
            };

        float mIfps;

        DEFINE_MEMBER_PTR_CONST(CrossSectionAnalyzer, CrossSectionAnalyzer);
        DEFINE_MEMBER(bool, CrossSectionEnabled, false);

        DEFINE_MEMBER(float, BlurThreshold, 5000.0f);
        DEFINE_MEMBER(int, MaxSamples, 2);

        DEFINE_MEMBER(bool, EnableAces, false);
        DEFINE_MEMBER(float, Exposure, 0.4f);
    };
};
