#pragma once

#include <QObject>

namespace Canavar::Engine
{
    struct Mouse
    {
            float x{ 0 };
            float y{ 0 };
            float dx{ 0 };
            float dy{ 0 };

            Qt::MouseButton button{ Qt::NoButton };

            void Reset()
            {
                x = 0;
                y = 0;
                dx = 0;
                dy = 0;
                button = Qt::NoButton;
            }
    };

} // namespace Canavar::Engine
