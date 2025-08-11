#include "EventReceiver.h"

void Canavar::Engine::EventReceiver::Initialize() {}

void Canavar::Engine::EventReceiver::Resize(int w, int h) {}

void Canavar::Engine::EventReceiver::Update(float ifps) {}

void Canavar::Engine::EventReceiver::Render(float ifps) {}

void Canavar::Engine::EventReceiver::PostRender(float ifps) {}

bool Canavar::Engine::EventReceiver::KeyPressed(QKeyEvent *)
{
    return false;
}

bool Canavar::Engine::EventReceiver::KeyReleased(QKeyEvent *)
{
    return false;
}

bool Canavar::Engine::EventReceiver::MousePressed(QMouseEvent *)
{
    return false;
}

bool Canavar::Engine::EventReceiver::MouseReleased(QMouseEvent *)
{
    return false;
}

bool Canavar::Engine::EventReceiver::MouseMoved(QMouseEvent *)
{
    return false;
}

bool Canavar::Engine::EventReceiver::WheelMoved(QWheelEvent *)
{
    return false;
}
