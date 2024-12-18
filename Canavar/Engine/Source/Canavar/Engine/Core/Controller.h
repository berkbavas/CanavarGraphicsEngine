#pragma once

#include "Canavar/Engine/Core/Constants.h"
#include "Canavar/Engine/Manager/Manager.h"

#include <QMouseEvent>
#include <QObject>
#include <QOpenGLExtraFunctions>
#include <QVector>

namespace Canavar::Engine
{
    class Window;
    class Widget;

    class NodeManager;
    class ShaderManager;
    class CameraManager;
    class RenderingManager;
    class LightManager;
    class VertexPainter;
    class EventReceiver;

    enum class ContainerMode
    {
        Widget,
        Window
    };

    class Controller : public QObject, public ManagerProvider
    {
        Q_OBJECT
      public:
        explicit Controller(ContainerMode mode = ContainerMode::Window, QObject* parent = nullptr);
        ~Controller();

        void Run();

        NodeManager* GetNodeManager() override { return mNodeManager; }
        ShaderManager* GetShaderManager() override { return mShaderManager; }
        CameraManager* GetCameraManager() override { return mCameraManager; }
        LightManager* GetLightManager() override { return mLightManager; }
        RenderingManager* GetRenderingManager() override { return mRenderingManager; }
        VertexPainter* GetVertexPainter() override { return mVertexPainter; }

        Window* GetWindow() { return mWindow; }
        Widget* GetWidget() { return mWidget; }

        void AddEventReceiver(EventReceiver* pReceiver);
        void RemoveEventReceiver(EventReceiver* pReceiver);

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

        void onRenderLoop(float ifps);

      private:
        ContainerMode mContainerMode{ ContainerMode::Window };
        Window* mWindow{ nullptr };
        Widget* mWidget{ nullptr };

        CameraManager* mCameraManager;
        NodeManager* mNodeManager;
        ShaderManager* mShaderManager;
        RenderingManager* mRenderingManager;
        LightManager* mLightManager;
        VertexPainter* mVertexPainter;

        QVector<Manager*> mManagers;

        QVector<EventReceiver*> mEventReceivers;

        float mDevicePixelRatio{ 1.0f };
        float mWidth{ INITIAL_WIDTH };
        float mHeight{ INITIAL_HEIGHT };

        bool mCaptureMouse{ false };
        bool mBlockNextMouseMoveEvent{ false };
    };
}
