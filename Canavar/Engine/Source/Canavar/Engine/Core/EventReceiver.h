#pragma once

#include <QMouseEvent>

namespace Canavar::Engine
{
    class EventReceiver
    {
      public:
        // Core Events
        virtual void Initialize();
        virtual void PostInitialize();
        virtual void Resize(int w, int h);
        virtual void Update(float ifps);
        virtual void Render(float ifps);
        virtual void InRender(float ifps);
        virtual void PostRender(float ifps);
        virtual void DrawImGui(float ifps);
        virtual void Shutdown();

        // Input Events
        // Returns true if the event is consumed so that it should not be dispatched others anymore.
        virtual bool KeyPressed(QKeyEvent *);
        virtual bool KeyReleased(QKeyEvent *);
        virtual bool MousePressed(QMouseEvent *);
        virtual bool MouseReleased(QMouseEvent *);
        virtual bool MouseMoved(QMouseEvent *);
        virtual bool WheelMoved(QWheelEvent *);
    };
}