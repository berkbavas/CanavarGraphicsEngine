#pragma once

#include "Aircraft.h"
#include "Canavar/Engine/Core/EventReceiver.h"

#include <QtImGui.h>

namespace Canavar::Engine
{
    class NodeManager;
    class CameraManager;
    class OpenGLWidget;
    class Controller;
    class Renderer;
    class DummyObject;
    class TexturedModel;
    class FreeCamera;
    class DummyCamera;
    class PersecutorCamera;
    class ImGuiWidget;
}

namespace Canavar::Simulator
{
    class Simulator : public QObject, public Canavar::Engine::EventReceiver
    {
      public:
        Simulator();
        ~Simulator();

        void Run();
        bool OnKeyPressed(QKeyEvent *pEvent) override;
        bool OnKeyReleased(QKeyEvent *pEvent) override;

      private:
        void Initialize();
        void Update(float Ifps);
        void OnPostRender(float Ifps);
        void DrawImGui();

        Aircraft *mAircraft;

        Canavar::Engine::OpenGLWidget *mWidget;

        Canavar::Engine::Renderer *mRenderer;
        Canavar::Engine::NodeManager *mNodeManager;
        Canavar::Engine::CameraManager *mCameraManager;

        Canavar::Engine::DummyObject *mRootNode;
        Canavar::Engine::TexturedModel *mJetNode;

        Canavar::Engine::FreeCamera *mFreeCamera;
        Canavar::Engine::DummyCamera *mDummyCamera;
        Canavar::Engine::PersecutorCamera *mPersecutorCamera;

        Canavar::Engine::ImGuiWidget *mImGuiWidget;

        QtImGui::RenderRef mRenderRef;
    };
}
