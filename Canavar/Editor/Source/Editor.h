#pragma once

#include "ImGuiWidget.h"
#include "Simulator/Simulator.h"

#include <Canavar/Engine/Core/Controller.h>
#include <Canavar/Engine/Core/EventReceiver.h>
#include <Canavar/Engine/Manager/CameraManager.h>
#include <Canavar/Engine/Manager/NodeManager.h>
#include <Canavar/Engine/Node/Camera/DummyCamera.h>
#include <Canavar/Engine/Node/Camera/FreeCamera.h>
#include <Canavar/Engine/Node/Camera/PersecutorCamera.h>

namespace Canavar::Editor
{
    class Contoller;

    class Editor : public QObject, public Engine::EventReceiver
    {
        Q_OBJECT
      public:
        Editor();

        void Run();

      public:
        // Core Events
        void Initialize() override;
        void Update(float ifps) override;

        // Input Events
        // Returns true if the event is consumed so that it should not be dispatched others anymore.
        bool KeyPressed(QKeyEvent *) override;
        bool KeyReleased(QKeyEvent *) override;
        bool MousePressed(QMouseEvent *) override;
        bool MouseReleased(QMouseEvent *) override;
        bool MouseMoved(QMouseEvent *) override;
        bool WheelMoved(QWheelEvent *) override;

      private:
        void CreateSimulatorModels();
        void Test();

        Engine::Controller *mController;
        Engine::NodeManager *mNodeManager;
        Engine::CameraManager *mCameraManager;

        ImGuiWidget *mImGuiWidget;

        Engine::FreeCameraPtr mFreeCamera;
        Engine::PersecutorCameraPtr mPersecutorCamera;
        Engine::DummyCameraPtr mDummyCamera;

        Simulator *mSimulator;
    };
}