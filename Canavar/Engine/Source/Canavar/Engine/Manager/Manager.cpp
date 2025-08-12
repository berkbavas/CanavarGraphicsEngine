#include "Manager.h"

Canavar::Engine::Manager::Manager(QObject *pParent)
    : QObject(pParent)
{}

void Canavar::Engine::Manager::PostInitialize() {}

void Canavar::Engine::Manager::Shutdown() {}

void Canavar::Engine::Manager::Update(float ifps) {}

void Canavar::Engine::Manager::Render(float ifps) {}
