#pragma once

#include "PerspectiveCamera.h"
#include "NodeManager.h"

namespace Canavar {
    namespace Engine {
        class DummyCamera : public PerspectiveCamera
        {
        protected:
            friend class Canavar::Engine::NodeManager;
            DummyCamera();

        public:
            void MouseDoubleClicked(QMouseEvent*) override;
            void MousePressed(QMouseEvent*) override;
            void MouseReleased(QMouseEvent*) override;
            void MouseMoved(QMouseEvent*) override;
            void WheelMoved(QWheelEvent*) override;
            void KeyPressed(QKeyEvent*) override;
            void KeyReleased(QKeyEvent*) override;
            void Update(float) override;
            void Reset() override;
        };
    } // namespace Engine
} // namespace Canavar
