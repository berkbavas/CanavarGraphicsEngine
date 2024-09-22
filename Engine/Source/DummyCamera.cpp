#include "DummyCamera.h"

Canavar::Engine::DummyCamera::DummyCamera()
    : PerspectiveCamera()
{
    mType = Canavar::Engine::Node::NodeType::DummyCamera;
}
 
void Canavar::Engine::DummyCamera::MouseDoubleClicked(QMouseEvent*) {}

void Canavar::Engine::DummyCamera::MousePressed(QMouseEvent*) {}

void Canavar::Engine::DummyCamera::MouseReleased(QMouseEvent*) {}

void Canavar::Engine::DummyCamera::MouseMoved(QMouseEvent*) {}

void Canavar::Engine::DummyCamera::WheelMoved(QWheelEvent*) {}

void Canavar::Engine::DummyCamera::KeyPressed(QKeyEvent*) {}

void Canavar::Engine::DummyCamera::KeyReleased(QKeyEvent*) {}

void Canavar::Engine::DummyCamera::Update(float) {}

void Canavar::Engine::DummyCamera::Reset() {}