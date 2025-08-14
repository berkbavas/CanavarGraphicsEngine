#pragma once

#include "Canavar/Engine/Core/Constants.h"
#include "Canavar/Engine/Core/RenderingContext.h"
#include "Canavar/Engine/Manager/Manager.h"
#include "Canavar/Engine/Util/ImGuiWidget.h"

#include <QMouseEvent>
#include <QObject>
#include <QOpenGLExtraFunctions>
#include <QVector>
#include <QtImGui.h>

namespace Canavar::Engine
{
    class Window;
    class Widget;

    class NodeManager;
    class ShaderManager;
    class CameraManager;
    class RenderingManager;
    class LightManager;
    class EventReceiver;
    class Window;
    class Widget;
    class Painter;
    class ShadowMappingRenderer;
    class TextRenderer;
    class BoundingBoxRenderer;

    class Controller : public QObject
    {
        Q_OBJECT
      public:
        explicit Controller(RenderingContext* pRenderingContext, bool withImGui, QObject* pParent = nullptr);
        ~Controller();

        void AddEventReceiver(EventReceiver* pReceiver);
        void RemoveEventReceiver(EventReceiver* pReceiver);

      signals:
        void Initialized();
        void PostInitialized();
        void Updated(float ifps);
        void PostRendered(float ifps);
        void DrawImGui(float ifps);  

      private slots:
        // Core Events
        void Initialize();
        void Resize(int w, int h);
        void Render(float ifps);

        // Input Events
        void OnKeyPressed(QKeyEvent*);
        void OnKeyReleased(QKeyEvent*);
        void OnMousePressed(QMouseEvent*);
        void OnMouseReleased(QMouseEvent*);
        void OnMouseMoved(QMouseEvent*);
        void OnWheelMoved(QWheelEvent*);
        void OnRenderLoop();

      private:
        RenderingContext* mRenderingContext{ nullptr };
        ImGuiWidget* mImGuiWidget{ nullptr };
        CameraManager* mCameraManager{ nullptr };
        NodeManager* mNodeManager{ nullptr };
        ShaderManager* mShaderManager{ nullptr };
        RenderingManager* mRenderingManager{ nullptr };
        LightManager* mLightManager{ nullptr };
        Painter* mPainter{ nullptr };
        ShadowMappingRenderer* mShadowMappingRenderer{ nullptr };
        TextRenderer* mTextRenderer{ nullptr };
        BoundingBoxRenderer* mBoundingBoxRenderer{ nullptr };

        QVector<Manager*> mManagers;
        QVector<EventReceiver*> mEventReceivers;

        float mDevicePixelRatio{ 1.0f };
        float mWidth{ INITIAL_WIDTH };
        float mHeight{ INITIAL_HEIGHT };

        QtImGui::RenderRef mRenderRef;
        Window* mWindow{ nullptr };
        Widget* mWidget{ nullptr };
    };
}
