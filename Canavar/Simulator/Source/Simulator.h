#pragma once

#include "Aircraft.h"

#include <Canavar/Engine/Core/Controller.h>
#include <Canavar/Engine/Core/EventReceiver.h>
#include <Canavar/Engine/Core/Widget.h>
#include <Canavar/Engine/Core/Window.h>
#include <Canavar/Engine/Manager/CameraManager.h>
#include <Canavar/Engine/Manager/LightManager.h>
#include <Canavar/Engine/Manager/NodeManager.h>
#include <Canavar/Engine/Manager/RenderingManager/RenderingManager.h>
#include <Canavar/Engine/Manager/ShaderManager.h>
#include <Canavar/Engine/Node/Object/Camera/DummyCamera.h>
#include <Canavar/Engine/Node/Object/Camera/FreeCamera.h>
#include <Canavar/Engine/Node/Object/Camera/PersecutorCamera.h>
#include <Canavar/Engine/Node/Object/DummyObject/DummyObject.h>
#include <Canavar/Engine/Node/Object/Light/PointLight.h>
#include <Canavar/Engine/Node/Object/Model/Model.h>
#include <Canavar/Engine/Util/ImGuiWidget.h>
#include <Canavar/Engine/Util/Logger.h>

#include <QtImGui.h>

namespace Canavar::Simulator
{
    class Simulator : public QObject, public Canavar::Engine::EventReceiver
    {
      public:
        Simulator();

        void Run();

        // Core Events
        void Initialize() override;
        void Update(float ifps) override;
        void PostRender(float ifps) override;

        // Input Events
        // Returns true if the event is consumed so that it should not be dispatched others anymore.
        bool KeyPressed(QKeyEvent *) override;
        bool KeyReleased(QKeyEvent *) override;

      private:
        QtImGui::RenderRef mRenderRef;
        Canavar::Engine::Window *mRendererContext;

        Canavar::Engine::Controller *mController;
        Canavar::Engine::NodeManager *mNodeManager;
        Canavar::Engine::CameraManager *mCameraManager;
        Canavar::Engine::ImGuiWidget *mImGuiWidget;

        Aircraft *mAircraft;

        Canavar::Engine::DummyObjectPtr mRootNode;
        Canavar::Engine::ModelPtr mJetNode;

        Canavar::Engine::FreeCameraPtr mFreeCamera;
        Canavar::Engine::DummyCameraPtr mDummyCamera;
        Canavar::Engine::PersecutorCameraPtr mPersecutorCamera;
    };
}
