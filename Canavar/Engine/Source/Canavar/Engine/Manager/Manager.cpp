#include "Manager.h"

Canavar::Engine::Manager::Manager(RenderingContext *pRenderingContext, QObject *pParent)
    : QObject(pParent)
    , mRenderingContext(pRenderingContext)
{}

void Canavar::Engine::Manager::Initialize() {}

void Canavar::Engine::Manager::PostInitialize() {}

void Canavar::Engine::Manager::Resize(int w, int h) {}

void Canavar::Engine::Manager::Update(float ifps) {}

void Canavar::Engine::Manager::Render(PerspectiveCamera *pActiveCamera) {}

void Canavar::Engine::Manager::InRender(PerspectiveCamera *pActiveCamera) {}

void Canavar::Engine::Manager::PostRender(float ifps) {}

void Canavar::Engine::Manager::Shutdown() {}
