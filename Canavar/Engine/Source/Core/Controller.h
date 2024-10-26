#pragma once

#include "Core/ManagerProvider.h"
#include "Manager/Manager.h"

#include <QMouseEvent>
#include <QObject>
#include <QOpenGLExtraFunctions>

namespace Canavar::Engine
{
    class Window;

    class NodeManager;
    class ShaderManager;
    class CameraManager;
    class RenderingManager;
    class LightManager;

    class Controller : public QObject, protected QOpenGLExtraFunctions, public ManagerProvider
    {
        Q_OBJECT
      public:
        explicit Controller(QObject* parent = nullptr);
        ~Controller();

        void Run();

        NodeManager* GetNodeManager() override { return mNodeManager; }
        ShaderManager* GetShaderManager() override { return mShaderManager; }
        CameraManager* GetCameraManager() override { return mCameraManager; }
        LightManager* GetLightManager() override { return mLightManager; }
        RenderingManager* GetRenderingManager() override { return mRenderingManager; }

      public slots:
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

      private:
        Window* mWindow;

        CameraManager* mCameraManager;
        NodeManager* mNodeManager;
        ShaderManager* mShaderManager;
        RenderingManager* mRenderingManager;
        LightManager* mLightManager;

        std::vector<Manager*> mManagers;
    };
}
