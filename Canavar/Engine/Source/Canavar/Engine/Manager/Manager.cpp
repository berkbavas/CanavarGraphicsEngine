#include "Manager.h"

Canavar::Engine::Manager::Manager(QObject *parent)
    : QObject(parent)
{}

void Canavar::Engine::Manager::PostInitialize() {}

void Canavar::Engine::Manager::PreUpdate(float ifps) {}

void Canavar::Engine::Manager::Update(float ifps) {}

void Canavar::Engine::Manager::Render(float ifps) {}

void Canavar::Engine::Manager::PostUpdate(float ifps) {}
