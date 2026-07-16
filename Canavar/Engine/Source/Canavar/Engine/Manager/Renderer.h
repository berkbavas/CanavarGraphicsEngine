#pragma once

#include "Canavar/Engine/Core/Framebuffer.h"
#include "Canavar/Engine/Core/OpenGLWidget.h"
#include "Canavar/Engine/Core/Quad.h"
#include "Canavar/Engine/Core/RenderingContext.h"
#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/Manager/CameraManager.h"
#include "Canavar/Engine/Manager/LightManager.h"
#include "Canavar/Engine/Manager/NodeManager.h"
#include "Canavar/Engine/Manager/TexturedModelRenderer.h"
#include "Canavar/Engine/Util/Macros.h"
#include "Canavar/Engine/Camera/PerspectiveCamera.h"

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

        OpenGLWidget* GetOpenGLWidget() const;
        NodeManager* GetNodeManager() const;
        LightManager* GetLightManager() const;
        CameraManager* GetCameraManager() const;
        TexturedModelRenderer* GetTexturedModelRenderer() const;

        PerspectiveCamera* GetActiveCamera() const;

        void AddEventReceiver(EventReceiver* pEventReceiver);
        void RemoveEventReceiver(EventReceiver* pEventReceiver);

      signals:
        void Initialized();
        void Resized(int Width, int Height);
        void CanRenderOverlay(float Ifps);

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
        void CreateDirectionalLights();

        OpenGLWidget* mOpenGLWidget{ nullptr };
        RenderingContext* mRenderingContext{ nullptr };

        NodeManagerPtr mNodeManager{ nullptr };
        LightManagerPtr mLightManager{ nullptr };
        CameraManagerPtr mCameraManager{ nullptr };
        TexturedModelRendererPtr mTexturedModelRenderer{ nullptr };

        ShaderPtr mScreenShader{ nullptr };
        QuadPtr mQuad{ nullptr };

        QList<Manager*> mManagers;
        QList<EventReceiver*> mEventReceivers;

        std::map<FramebufferType, FramebufferPtr> mFramebuffers;
        std::map<FramebufferType, QOpenGLFramebufferObjectFormat> mFramebufferFormats;
        std::map<FramebufferType, QVector<GLenum>> mFramebufferExtraColorAttachments;

        int mWidth{ 1600 };
        int mHeight{ 900 };
        float mDevicePixelRatio{ 1.0f };

        DEFINE_MEMBER(QVector3D, ClearColor, QVector3D(0.0f, 0.0f, 0.0f));
    };

    using RendererPtr = std::unique_ptr<Renderer>;
}