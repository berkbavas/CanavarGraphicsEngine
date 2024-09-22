#include "Mouse.h"

Canavar::Engine::Mouse::Mouse()
    : mPressedButton(Qt::NoButton)
    , mX(0)
    , mY(0)
    , mZ(0)
    , mDx(0)
    , mDy(0)
    , mDz(0)
{}

void Canavar::Engine::Mouse::Reset()
{
    mPressedButton = Qt::NoButton;
    mX = 0;
    mY = 0;
    mZ = 0;
    mDx = 0;
    mDy = 0;
    mDz = 0;
}