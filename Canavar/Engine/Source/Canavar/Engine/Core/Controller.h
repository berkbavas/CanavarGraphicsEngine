#pragma once

#include "Canavar/Engine/Core/Constants.h"
#include "Canavar/Engine/Core/RenderingContext.h"
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
    class EventReceiver;

    class Controller : public QObject, public ManagerProvider
    {
        Q_OBJECT
      public:
        explicit Controller(RenderingContext* pRenderingContext, QObject* pParent = nullptr);
        ~Controller();

        NodeManager* GetNodeManager() override;
        ShaderManager* GetShaderManager() override;
        CameraManager* GetCameraManager() override;
        LightManager* GetLightManager() override;
        RenderingManager* GetRenderingManager() override;

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

        void OnRenderLoop(float ifps);

      private:
        RenderingContext* mRenderingContext{ nullptr };

        CameraManager* mCameraManager;
        NodeManager* mNodeManager;
        ShaderManager* mShaderManager;
        RenderingManager* mRenderingManager;
        LightManager* mLightManager;

        QVector<Manager*> mManagers;

        QVector<EventReceiver*> mEventReceivers;

        float mDevicePixelRatio{ 1.0f };
        float mWidth{ INITIAL_WIDTH };
        float mHeight{ INITIAL_HEIGHT };
    };
}
