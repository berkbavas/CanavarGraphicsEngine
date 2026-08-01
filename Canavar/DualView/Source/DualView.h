#pragma once

#include "Canavar/Engine/Core/OpenGLWidget.h"
#include "Canavar/Engine/Manager/Renderer.h"
#include "Canavar/Engine/Util/ImGuiWidget.h"

namespace Canavar::DualView
{
    class DualView
    {
      public:
        DualView();
        ~DualView();

        void Run();

      private:
        QWidget* mContainer{ nullptr };
        Canavar::Engine::OpenGLWidget* mLeftWidget{ nullptr };
        Canavar::Engine::OpenGLWidget* mRightWidget{ nullptr };
        Canavar::Engine::RendererPtr mLeftRenderer{ nullptr };
        Canavar::Engine::RendererPtr mRightRenderer{ nullptr };
        Canavar::Engine::ImGuiWidgetPtr mLeftImGui{ nullptr };
        Canavar::Engine::ImGuiWidgetPtr mRightImGui{ nullptr };
    };
}
