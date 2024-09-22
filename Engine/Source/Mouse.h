#pragma once

#include <QObject>

namespace Canavar {
    namespace Engine {
        class Mouse
        {
        public:
            Mouse();
            void Reset();

            Qt::MouseButton mPressedButton;
            float mX;
            float mY;
            float mZ;
            float mDx;
            float mDy;
            float mDz;
        };
    } // namespace Engine
} // namespace Canavar
