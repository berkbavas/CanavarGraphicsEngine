#pragma once

#include "Mouse.h"
#include "PerspectiveCamera.h"
#include "Speed.h"

namespace Canavar {
    namespace Engine {
        class FreeCamera : public PerspectiveCamera
        {
            Q_OBJECT
        protected:
            friend class NodeManager;
            FreeCamera();

        public:
            void MouseDoubleClicked(QMouseEvent*) override;
            void WheelMoved(QWheelEvent*) override;
            void MousePressed(QMouseEvent* event) override;
            void MouseReleased(QMouseEvent* event) override;
            void MouseMoved(QMouseEvent* event) override;
            void KeyPressed(QKeyEvent* event) override;
            void KeyReleased(QKeyEvent* event) override;
            void Update(float ifps) override;
            void Reset() override;

        private:
            QMap<Qt::Key, bool> mPressedKeys;

            Mouse mMouse;
            bool mUpdateRotation;
            bool mUpdatePosition;

            DEFINE_MEMBER(Speed, Speed);

            static const QMap<Qt::Key, QVector3D> KEY_BINDINGS;
        };
    } // namespace Engine
} // namespace Canavar