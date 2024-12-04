#include "Simulator.h"

#include <Canavar/Engine/Core/Window.h>

Canavar::Simulator::Simulator::Simulator()
{
    mController = new Canavar::Engine::Controller;
    mController->AddEventReceiver(this);
}

void Canavar::Simulator::Simulator::Run()
{
    mController->Run();
}

void Canavar::Simulator::Simulator::Initialize()
{
    QtImGui::initialize(mController->GetWindow());

    mNodeManager = mController->GetNodeManager();
    mCameraManager = mController->GetCameraManager();
    mPersecutorCamera = std::make_shared<Canavar::Engine::PersecutorCamera>();

    mAircraft = new Aircraft;
    mAircraftController = new AircraftController(mAircraft);

    // Post initialize
    mNodeManager->ImportNodes("Resources/Nodes.json");
    mFreeCamera = mCameraManager->GetFreeCamera();
    mRootNode = mNodeManager->FindNodeByName<Canavar::Engine::DummyObject>("Root Node");
    mJetNode = mNodeManager->FindNodeByName<Canavar::Engine::Model>("f16c");
    mDummyCamera = mNodeManager->FindNodeByName<Canavar::Engine::DummyCamera>("Dummy Camera");

    mCameraManager->SetActiveCamera(mPersecutorCamera);
    mPersecutorCamera->SetTarget(mRootNode);
    mPersecutorCamera->SetYaw(180.0f);
    mPersecutorCamera->SetPitch(-10.0f);

    mAircraftController->SetRootNode(mRootNode);
    mAircraftController->SetJetNode(mJetNode);

    mAircraftController->Initialize();
    mAircraft->Initialize();
}

void Canavar::Simulator::Simulator::PostRender(float ifps)
{
    mAircraftController->Update(ifps);

    QtImGui::newFrame();
    mAircraftController->DrawGui();
    ImGui::Render();
    QtImGui::render();
}

bool Canavar::Simulator::Simulator::KeyPressed(QKeyEvent* pEvent)
{
    mAircraftController->KeyPressed(pEvent);

    if (pEvent->key() == Qt::Key_1)
    {
        mCameraManager->SetActiveCamera(mFreeCamera);
    }
    else if (pEvent->key() == Qt::Key_2)
    {
        mCameraManager->SetActiveCamera(mPersecutorCamera);
    }
    else if (pEvent->key() == Qt::Key_3)
    {
        mCameraManager->SetActiveCamera(mDummyCamera);
        return true;
    }

    return false;
}

bool Canavar::Simulator::Simulator::KeyReleased(QKeyEvent* pEvent)
{
    mAircraftController->KeyReleased(pEvent);

    return false;
}
