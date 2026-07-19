#pragma once

#include "Canavar/Engine/Camera/PerspectiveCamera.h"
#include "Canavar/Engine/Core/EventThief.h"
#include "Canavar/Engine/Core/Framebuffer.h"
#include "Canavar/Engine/Core/OpenGLWidget.h"
#include "Canavar/Engine/Core/Quad.h"
#include "Canavar/Engine/Core/RenderingContext.h"
#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/GlobalNode/Haze/Haze.h"
#include "Canavar/Engine/GlobalNode/Sky/Sky.h"
#include "Canavar/Engine/GlobalNode/Terrain/Terrain.h"
#include "Canavar/Engine/Manager/CameraManager.h"
#include "Canavar/Engine/Manager/LightManager.h"
#include "Canavar/Engine/Manager/NodeManager.h"
#include "Canavar/Engine/Manager/PrimitiveModelRenderer.h"
#include "Canavar/Engine/Manager/TexturedModelRenderer.h"
#include "Canavar/Engine/PostProcessEffect/AcesEffect.h"
#include "Canavar/Engine/PostProcessEffect/ChromaticAberrationEffect.h"
#include "Canavar/Engine/PostProcessEffect/CinematicEffect.h"
#include "Canavar/Engine/PostProcessEffect/ColorGradingEffect.h"
#include "Canavar/Engine/PostProcessEffect/DepthOfFieldEffect.h"
#include "Canavar/Engine/PostProcessEffect/FxaaEffect.h"
#include "Canavar/Engine/PostProcessEffect/LensDistortionEffect.h"
#include "Canavar/Engine/PostProcessEffect/SharpenEffect.h"
#include "Canavar/Engine/Util/Gizmo.h"
#include "Canavar/Engine/Util/Macros.h"

#include <memory>

#include <QObject>
#include <QOpenGLFunctions_4_5_Core>

namespace Canavar::Engine
{
    class Renderer : public QObject, protected QOpenGLFunctions_4_5_Core
    {
        Q_OBJECT
      public:
        explicit Renderer(OpenGLWidget* pOpenGLWidget);
        virtual ~Renderer() = default;

        // Main rendering function that handles the entire rendering process for a frame.
        void RenderToFramebuffer(Framebuffer* pFramebuffer, PerspectiveCamera* pCamera);

        // Accessor for scenes managed by the TexturedModelRenderer
        const QMap<QString, ScenePtr>& GetScenes() const;

        // Accessors for various managers and components
        OpenGLWidget* GetOpenGLWidget() const;
        NodeManager* GetNodeManager() const;
        LightManager* GetLightManager() const;
        CameraManager* GetCameraManager() const;

        // Accessors for global nodes
        Sky* GetSky() const;
        Haze* GetHaze() const;
        Terrain* GetTerrain() const;

        // Accessor for the Gizmo
        Gizmo* GetGizmo() const;

        // Post-processing effects
        AcesEffect* GetAcesEffect() const;
        DepthOfFieldEffect* GetDepthOfFieldEffect() const;
        FxaaEffect* GetFxaaEffect() const;
        ColorGradingEffect* GetColorGradingEffect() const;
        SharpenEffect* GetSharpenEffect() const;
        ChromaticAberrationEffect* GetChromaticAberrationEffect() const;
        LensDistortionEffect* GetLensDistortionEffect() const;
        CinematicEffect* GetCinematicEffect() const;

        bool GetPostProcessEffectEnabled(PostProcessEffectType Type) const;
        void SetPostProcessEffectEnabled(PostProcessEffectType Type, bool Enabled);

        PerspectiveCamera* GetActiveCamera() const;

        void AddEventReceiver(EventReceiver* pEventReceiver);
        void RemoveEventReceiver(EventReceiver* pEventReceiver);

        void AddEventThief(EventThief* pEventThief);
        void RemoveEventThief(EventThief* pEventThief);

      signals:
        void Initialized();
        void Updated(float Ifps);
        void Resized(int Width, int Height);
        void PostRender(float Ifps);

      private:
        void Initialize();
        void Resize(int Width, int Height);
        void Render(float Ifps);
        void OnKeyPressed(QKeyEvent* pEvent);
        void OnKeyReleased(QKeyEvent* pEvent);
        void OnMousePressed(QMouseEvent* pEvent);
        void OnMouseReleased(QMouseEvent* pEvent);
        void OnMouseMoved(QMouseEvent* pEvent);
        void OnWheelMoved(QWheelEvent* pEvent);
        void OnLeaveEvent(QEvent* pEvent);
        void ResizeFramebuffers(int Width, int Height);
        void CreateFreeCameraIfAbsent();
        void CreatePersecutorCameraIfAbsent();
        void CreateDirectionalLights();
        void CreateGlobalNodes();
        void ApplyPostProcessEffects();
        void SupplyPerFrameData();

        OpenGLWidget* mOpenGLWidget{ nullptr };
        RenderingContext* mRenderingContext{ nullptr };

        NodeManagerPtr mNodeManager{ nullptr };
        LightManagerPtr mLightManager{ nullptr };
        CameraManagerPtr mCameraManager{ nullptr };
        TexturedModelRendererPtr mTexturedModelRenderer{ nullptr };
        PrimitiveModelRendererPtr mPrimitiveModelRenderer{ nullptr };

        ShaderPtr mScreenShader{ nullptr };
        QuadPtr mQuad{ nullptr };

        QList<Manager*> mManagers;
        QList<EventReceiver*> mEventReceivers;
        QList<EventThief*> mEventThieves;

        std::map<FramebufferType, FramebufferPtr> mFramebuffers;
        std::map<FramebufferType, QOpenGLFramebufferObjectFormat> mFramebufferFormats;
        std::map<FramebufferType, QVector<GLenum>> mFramebufferExtraColorAttachments;

        std::map<PostProcessEffectType, bool> mPostProcessEffectEnabled;
        std::map<PostProcessEffectType, PostProcessEffect*> mPostProcessEffects;

        AcesEffectPtr mAcesEffect{ nullptr };
        DepthOfFieldEffectPtr mDepthOfFieldEffect{ nullptr };
        ColorGradingEffectPtr mColorGradingEffect{ nullptr };
        SharpenEffectPtr mSharpenEffect{ nullptr };
        FxaaEffectPtr mFxaaEffect{ nullptr };
        ChromaticAberrationEffectPtr mChromaticAberrationEffect{ nullptr };
        LensDistortionEffectPtr mLensDistortionEffect{ nullptr };
        CinematicEffectPtr mCinematicEffect{ nullptr };

        Sky* mSky{ nullptr };
        Haze* mHaze{ nullptr };
        Terrain* mTerrain{ nullptr };

        // Gizmo for object manipulation
        GizmoPtr mGizmo{ nullptr };

        int mWidth{ 1600 };
        int mHeight{ 900 };
        float mDevicePixelRatio{ 1.0f };

        DEFINE_MEMBER(QVector3D, ClearColor, QVector3D(0.0f, 0.0f, 0.0f));
        DEFINE_MEMBER_PTR_CONST(DirectionalLight, Sun);
    };

    using RendererPtr = std::unique_ptr<Renderer>;
}