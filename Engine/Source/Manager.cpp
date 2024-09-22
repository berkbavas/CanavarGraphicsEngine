#include "Manager.h"

Canavar::Engine::Manager::Manager(QObject* parent)
    : QObject(parent)
{}

void Canavar::Engine::Manager::PostInit() {}

void Canavar::Engine::Manager::MouseDoubleClicked(QMouseEvent*) {}

void Canavar::Engine::Manager::MousePressed(QMouseEvent*) {}

void Canavar::Engine::Manager::MouseReleased(QMouseEvent*) {}

void Canavar::Engine::Manager::MouseMoved(QMouseEvent*) {}

void Canavar::Engine::Manager::WheelMoved(QWheelEvent*) {}

void Canavar::Engine::Manager::KeyPressed(QKeyEvent*) {}

void Canavar::Engine::Manager::KeyReleased(QKeyEvent*) {}

void Canavar::Engine::Manager::Resize(int, int) {}

void Canavar::Engine::Manager::Update(float) {}

void Canavar::Engine::Manager::Render(float) {}

void Canavar::Engine::Manager::Reset() {}