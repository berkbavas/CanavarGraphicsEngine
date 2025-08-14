#pragma once

#include "Canavar/Engine/Core/Constants.h"
#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/Core/Structs.h"
#include "Canavar/Engine/Manager/Manager.h"
#include "Canavar/Engine/Node/Global/Haze/Haze.h"
#include "Canavar/Engine/Node/Global/Sky/Sky.h"
#include "Canavar/Engine/Node/Global/Terrain/Terrain.h"
#include "Canavar/Engine/Node/Object/Camera/PerspectiveCamera.h"
#include "Canavar/Engine/Node/Object/Effect/NozzleEffect/NozzleEffect.h"
#include "Canavar/Engine/Node/Object/Light/DirectionalLight.h"
#include "Canavar/Engine/Node/Object/Model/Model.h"

#include <map>

#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLFunctions_4_5_Core>


namespace Canavar::Engine
{
    class ShaderManager;
    class NodeManager;
    class CameraManager;
    class LightManager;
    class BoundingBoxRenderer;
    class TextRenderer;
    class ShadowMappingRenderer;

    enum Framebuffer
    {
        Multisample,
        Singlesample,
        MotionBlur,
        Aces,
        Cinematic,
        Temp
    };

    class RenderingManager : public Manager, protected QOpenGLFunctions_4_5_Core
    {
        Q_OBJECT
      public:
        using Manager::Manager;

        void Initialize() override;
        void PostInitialize() override;
        void Shutdown() override;
        void Update(float ifps) override;
        void Render(PerspectiveCamera *pActiveCamera) override;
        void Resize(int width, int height);

        QVector3D FetchFragmentLocalPositionFromScreen(int x, int y);
        QVector3D FetchFragmentWorldPositionFromScreen(int x, int y);
        NodeInfo FetchNodeInfoFromScreenCoordinates(int x, int y);

        void RenderToFramebuffer(QOpenGLFramebufferObject *pFramebuffer, PerspectiveCamera *pCamera);

      signals:
        void RenderLoop();

      private:
        void RenderObjects(PerspectiveCamera *pCamera);
        void RenderModel(Model *pModel, RenderPass RenderPass);
        void RenderNozzleEffect(NozzleEffect *pEffect, PerspectiveCamera *pCamera);
        void RenderSky(PerspectiveCamera *pCamera);
        void RenderTerrain(Terrain *pTerrain, PerspectiveCamera *pCamera);

        void SetUniforms(PerspectiveCamera *pCamera);
        void SetCommonUniforms(Shader *pShader, PerspectiveCamera *pCamera);
        void SetDirectionalLights(Shader *pShader);
        void SetPointLights(Shader *pShader, Object *pObject);

        void ResizeFramebuffers();
        void DoPostProcessing();

        QVector2D CalculateSunScreenSpacePosition() const;

        void ApplyAcesPass();
        void ApplyCinematicPass();
        void ApplyMotionBlurPass();

        void DestroyFramebuffers();

        ShaderManager *mShaderManager;
        NodeManager *mNodeManager;
        CameraManager *mCameraManager;
        LightManager *mLightManager;
        ShadowMappingRenderer *mShadowMappingRenderer{ nullptr };

        PerspectiveCamera *mActiveCamera{ nullptr };

        Shader *mModelShader{ nullptr };
        Shader *mSkyShader{ nullptr };
        Shader *mBlurShader{ nullptr };
        Shader *mPostProcessShader{ nullptr };
        Shader *mNozzleEffectShader{ nullptr };
        Shader *mLightningStrikeShader{ nullptr };
        Shader *mLineShader{ nullptr };
        Shader *mTerrainShader{ nullptr };
        Shader *mCinematicShader{ nullptr };
        Shader *mBrightPassShader{ nullptr };
        Shader *mGodRaysShader{ nullptr };
        Shader *mCompositionShader{ nullptr };
        Shader *mAcesShader{ nullptr };
        Shader *mMotionBlurShader{ nullptr };

        SkyPtr mSky;
        DirectionalLightPtr mSun;
        TerrainPtr mTerrain;
        HazePtr mHaze;

        Quad *mQuad;

        std::map<Framebuffer, QOpenGLFramebufferObject *> mFramebuffers;
        std::map<Framebuffer, QOpenGLFramebufferObjectFormat> mFramebufferFormats;
        static constexpr std::array<Framebuffer, 6> FBO_TYPES = { Multisample, Singlesample, MotionBlur, Aces, Cinematic, Temp };

        int mWidth{ INITIAL_WIDTH };
        int mHeight{ INITIAL_HEIGHT };

        static constexpr int NUMBER_OF_FBO_ATTACHMENTS = 5;
        static constexpr GLuint FBO_ATTACHMENTS[] = //
            {
                GL_COLOR_ATTACHMENT0, // Color
                GL_COLOR_ATTACHMENT1, // Fragment local position
                GL_COLOR_ATTACHMENT2, // Fragment world position
                GL_COLOR_ATTACHMENT3, // Node info
                GL_COLOR_ATTACHMENT4, // Fragment velocity and depth
            };

        float mIfps;
        float mTime{ 0.0f };

        // ACES
        DEFINE_MEMBER(bool, AcesEnabled, false);

        // Cinematic Look
        DEFINE_MEMBER(bool, CinematicEnabled, true);
        DEFINE_MEMBER(float, VignetteRadius, 0.95f);
        DEFINE_MEMBER(float, VignetteSoftness, 0.5f);
        DEFINE_MEMBER(float, GrainStrength, 0.00f);

        // God Rays
        DEFINE_MEMBER(float, BrightnessThreshold, 0.5f);
        DEFINE_MEMBER(int, NumberOfSamples, 100);
        DEFINE_MEMBER(float, Density, 0.1f);
        DEFINE_MEMBER(float, Decay, 0.95f);
        DEFINE_MEMBER(float, Weight, 0.1f);
        DEFINE_MEMBER(float, Exposure, 0.3f);

        // Motion Blur
        DEFINE_MEMBER(bool, MotionBlurEnabled, false);
        DEFINE_MEMBER(float, MotionBlurStrength, 0.15f);
        DEFINE_MEMBER(int, MotionBlurSamples, 20);
        DEFINE_MEMBER(float, MotionBlurDepthThreshold, 0.5f);

        // Mesh Selection
        DEFINE_MEMBER(bool, MeshSelectionEnabled, false);

        QQuaternion mPreviousRotation;
        QMatrix4x4 mPreviousViewMatrix;
        QMatrix4x4 mPreviousProjectionMatrix;
        QMatrix4x4 mPreviousViewProjectionMatrix;

        float mDevicePixelRatio{ 1.0f };
    };
};
