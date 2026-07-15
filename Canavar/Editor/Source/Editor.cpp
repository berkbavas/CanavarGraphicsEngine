#include "Editor.h"

Canavar::Editor::Editor::~Editor()
{
    mOpenGLWidget->MakeCurrent(); // Ensure the OpenGL context is current before destroying the renderer
    mRenderer = nullptr;
}

void Canavar::Editor::Editor::Run()
{
    mOpenGLWidget = new Canavar::Engine::OpenGLWidget(nullptr);
    mRenderer = std::make_unique<Canavar::Engine::Renderer>(mOpenGLWidget);

    mImGuiWidget = new Canavar::Engine::ImGuiWidget(mRenderer.get());

    mRenderer->AddEventReceiver(mImGuiWidget);

    mOpenGLWidget->showMaximized();
}
