#pragma once

#include "Canavar/Engine/Core/OpenGLWidget.h"
#include "Canavar/Engine/Manager/Renderer.h"
#include "Canavar/Engine/Util/ImGuiWidget.h"

namespace Canavar::Editor
{
    class Editor
    {
      public:
        Editor() = default;
        ~Editor();

        void Run();

      private:
        Canavar::Engine::OpenGLWidget *mOpenGLWidget{ nullptr };
        Canavar::Engine::RendererPtr mRenderer{ nullptr };
        Canavar::Engine::ImGuiWidget *mImGuiWidget{ nullptr };
    };
}