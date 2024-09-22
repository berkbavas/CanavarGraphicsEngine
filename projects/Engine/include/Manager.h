#pragma once

#include <QMouseEvent>
#include <QObject>

namespace Canavar {
    namespace Engine {
        class Manager : public QObject
        {
            Q_OBJECT
        public:
            explicit Manager(QObject* parent = nullptr);

            virtual bool Init() = 0;
            virtual void PostInit();
            virtual void MouseDoubleClicked(QMouseEvent* event);
            virtual void MousePressed(QMouseEvent* event);
            virtual void MouseReleased(QMouseEvent* event);
            virtual void MouseMoved(QMouseEvent* event);
            virtual void WheelMoved(QWheelEvent* event);
            virtual void KeyPressed(QKeyEvent* event);
            virtual void KeyReleased(QKeyEvent* event);
            virtual void Resize(int width, int height);
            virtual void Update(float ifps);
            virtual void Render(float ifps);
            virtual void Reset();
        };
    } // namespace Engine
} // namespace Canavar
