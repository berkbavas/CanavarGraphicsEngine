#pragma once

#include "ImGuiWidget.h"

#include <Canavar/Engine/Core/Controller.h>
#include <Canavar/Engine/Core/EventReceiver.h>
#include <Canavar/Engine/Manager/NodeManager.h>

namespace Canavar::Editor
{
    class Contoller;

    class Editor : Engine::EventReceiver
    {
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
        Engine::Controller *mController;
        Engine::NodeManager *mNodeManager;

        ImGuiWidget *mImGuiWidget;
    };
}