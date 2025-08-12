#include "Simulator.h"

#include <Canavar/Engine/Core/Window.h>
#include <Canavar/Engine/Node/Object/Text/Text2D.h>
#include <Canavar/Engine/Node/Object/Text/Text3D.h>

Canavar::Simulator::Simulator::Simulator()
{
    mWindow = new Canavar::Engine::Window(nullptr);
    mController = new Canavar::Engine::Controller(mWindow, true, this);

    mController->AddEventReceiver(this);

    mNodeManager = mController->GetNodeManager();
    mCameraManager = mController->GetCameraManager();

    mAircraft = new Aircraft;
}

Canavar::Simulator::Simulator::~Simulator()
{
    LOG_DEBUG("Simulator::~Simulator: Destructor called");
}

void Canavar::Simulator::Simulator::Run()
{
    if (mAircraft->Initialize() == false)
    {
        CGE_EXIT_FAILURE("Simulator::Run: Aircraft initialization failed. Exiting...");
    }

    mWindow->showMinimized();
}

void Canavar::Simulator::Simulator::PostInitialize()
{
    mPersecutorCamera = std::make_shared<Canavar::Engine::PersecutorCamera>();

    mNodeManager->ImportNodes("Resources/f16.json");
    mFreeCamera = mCameraManager->GetFreeCamera();
    mRootNode = mNodeManager->FindNodeByName<Canavar::Engine::DummyObject>("Root Node");
    mJetNode = mNodeManager->FindNodeByName<Canavar::Engine::Model>("f16");
    mDummyCamera = mNodeManager->FindNodeByName<Canavar::Engine::DummyCamera>("Dummy Camera");

    mCameraManager->SetActiveCamera(mPersecutorCamera);
    mPersecutorCamera->SetTarget(mRootNode);

    mWindow->showMaximized();
}

void Canavar::Simulator::Simulator::Update(float ifps)
{
    mAircraft->Tick(ifps);
}

void Canavar::Simulator::Simulator::PostRender(float ifps)
{
    const auto& pfd = mAircraft->GetPfd();
    mRootNode->SetWorldPosition(pfd.Position);
    mRootNode->SetWorldRotation(pfd.Rotation);
}

void Canavar::Simulator::Simulator::DrawImGui(float ifps)
{
    mAircraft->DrawGui();
}

bool Canavar::Simulator::Simulator::KeyPressed(QKeyEvent* pEvent)
{
    mAircraft->OnKeyPressed(pEvent);

    if (pEvent->key() == Qt::Key_1)
    {
        mCameraManager->SetActiveCamera(mFreeCamera);
        return true;
    }
    else if (pEvent->key() == Qt::Key_2)
    {
        mCameraManager->SetActiveCamera(mPersecutorCamera);
        return true;
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
    mAircraft->OnKeyReleased(pEvent);

    return false;
}
