#pragma once

#include "Mouse.h"
#include "PerspectiveCamera.h"
#include "Speed.h"

namespace Canavar {
    namespace Engine {
        class PersecutorCamera : public PerspectiveCamera
        {
        protected:
            friend class NodeManager;
            PersecutorCamera();

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

            Node* GetTarget() const;
            void SetTarget(Node* newTarget);

        private:
            Node* mTarget;

            Mouse mMouse;
            Speed mSpeed;

            DEFINE_MEMBER_CONST(float, Distance);
            DEFINE_MEMBER_CONST(float, Yaw);
            DEFINE_MEMBER_CONST(float, Pitch);
        };
    } // namespace Engine
} // namespace Canavar
