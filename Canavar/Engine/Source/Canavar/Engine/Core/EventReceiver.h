#pragma once

#include <QMouseEvent>

namespace Canavar::Engine
{
    class EventReceiver
    {
      public:
        // Returns true if the event is consumed so that it should not be dispatched others anymore.
        virtual bool KeyPressed(QKeyEvent *);
        virtual bool KeyReleased(QKeyEvent *);
        virtual bool MousePressed(QMouseEvent *);
        virtual bool MouseReleased(QMouseEvent *);
        virtual bool MouseMoved(QMouseEvent *);
        virtual bool WheelMoved(QWheelEvent *);
    };
}