#pragma once

#include <QObject>

namespace Canavar::Engine
{
    struct NodeInfo
    {
        float nodeId;
        float meshId;
        float primitiveId;
        float success;
    };

    struct Mouse
    {
        float x;
        float y;
        float dx;
        float dy;

        Qt::MouseButton button;

        Mouse() { Reset(); }

        void Reset()
        {
            x = 0;
            y = 0;
            dx = 0;
            dy = 0;
            button = Qt::NoButton;
        }
    };
}