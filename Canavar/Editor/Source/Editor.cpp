#include "Editor.h"

Canavar::Editor::Editor::~Editor()
{
    mOpenGLWidget->MakeCurrent(); // Ensure the OpenGL context is current before destroying the renderer
    mRenderer = nullptr;
}

void Canavar::Editor::Editor::Run()
{
    // We need to create the OpenGLWidget first, then the Renderer, and finally the ImGuiWidget.
    mOpenGLWidget = new Canavar::Engine::OpenGLWidget(nullptr);
    mRenderer = std::make_unique<Canavar::Engine::Renderer>(mOpenGLWidget);
    mImGuiWidget = std::make_unique<Canavar::Engine::ImGuiWidget>(mRenderer.get());

    // After calling this function, the OpenGLWidget will be shown maximized, and the main event loop will start.
    mOpenGLWidget->showMaximized();
}
