#pragma once

#include <Canavar/Engine/Core/Controller.h>
#include <Canavar/Engine/Core/EventReceiver.h>
#include <Canavar/Engine/Manager/CameraManager.h>
#include <Canavar/Engine/Manager/NodeManager.h>
#include <Canavar/Engine/Manager/RenderingManager/RenderingManager.h>
#include <Canavar/Engine/Node/Object/Camera/DummyCamera.h>
#include <Canavar/Engine/Node/Object/Camera/FreeCamera.h>
#include <Canavar/Engine/Node/Object/Camera/PersecutorCamera.h>
#include <Canavar/Engine/Node/Object/DummyObject/DummyObject.h>
#include <Canavar/Engine/Node/Object/Model/Model.h>
#include <Canavar/Engine/Util/ImGuiWidget.h>

namespace Canavar::Editor
{
    class Contoller;

    class Editor : public QObject, public Engine::EventReceiver
    {
        Q_OBJECT
      public:
        explicit Editor(QObject *pParent);
        ~Editor();

        void Run();

      public:
        // Core Events
        void Initialize() override;
        void Render(float ifps) override;
        void PostRender(float ifps) override;

        // Input Events
        // Returns true if the event is consumed so that it should not be dispatched others anymore.
        bool KeyPressed(QKeyEvent *) override;
        bool KeyReleased(QKeyEvent *) override;
        bool MousePressed(QMouseEvent *) override;
        bool MouseReleased(QMouseEvent *) override;
        bool MouseMoved(QMouseEvent *) override;
        bool WheelMoved(QWheelEvent *) override;

      private:
        Engine::Window *mWindow;

        Engine::Controller *mController;
        Engine::NodeManager *mNodeManager;
        Engine::CameraManager *mCameraManager;
        Engine::RenderingManager *mRenderingManager;

        Engine::ImGuiWidget *mImGuiWidget;

        Engine::FreeCameraPtr mFreeCamera;
    };
}