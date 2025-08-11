#include "Simulator.h"

#include <Canavar/Engine/Core/Window.h>
#include <Canavar/Engine/Node/Object/Text/Text2D.h>
#include <Canavar/Engine/Node/Object/Text/Text3D.h>

Canavar::Simulator::Simulator::Simulator()
{
    mController = new Canavar::Engine::Controller(Canavar::Engine::ContainerMode::Window);
    mRendererContext = mController->GetWindow();

    mImGuiWidget = new Canavar::Engine::ImGuiWidget;
    mImGuiWidget->SetRenderingManager(mController->GetRenderingManager());
    mImGuiWidget->SetNodeManager(mController->GetNodeManager());
    mImGuiWidget->SetCameraManager(mController->GetCameraManager());

    connect(mImGuiWidget,
            &Canavar::Engine::ImGuiWidget::GoToObject,
            this,
            [=](Canavar::Engine::ObjectPtr pObject) //
            {
                mController->GetCameraManager()->GetFreeCamera()->GoToObject(pObject);
                mController->GetCameraManager()->SetActiveCamera(mController->GetCameraManager()->GetFreeCamera());
            });

    mController->AddEventReceiver(mImGuiWidget);
    mController->AddEventReceiver(this);

    mAircraft = new Aircraft;
}

void Canavar::Simulator::Simulator::Run()
{
    if (mAircraft->Initialize() == false)
    {
        CGE_EXIT_FAILURE("Simulator::Run: Aircraft initialization failed. Exiting...");
    }

    mController->Run();
    mRendererContext->showMaximized();
}

void Canavar::Simulator::Simulator::Initialize()
{
    mRenderRef = QtImGui::initialize(mRendererContext);

    mNodeManager = mController->GetNodeManager();
    mCameraManager = mController->GetCameraManager();
    mPersecutorCamera = std::make_shared<Canavar::Engine::PersecutorCamera>();

    mNodeManager->ImportNodes("Resources/f16.json");
    mFreeCamera = mCameraManager->GetFreeCamera();
    mRootNode = mNodeManager->FindNodeByName<Canavar::Engine::DummyObject>("Root Node");
    mJetNode = mNodeManager->FindNodeByName<Canavar::Engine::Model>("f16");
    mDummyCamera = mNodeManager->FindNodeByName<Canavar::Engine::DummyCamera>("Dummy Camera");

    mCameraManager->SetActiveCamera(mPersecutorCamera);
    mPersecutorCamera->SetTarget(mRootNode);

    mImGuiWidget->Initialize();
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

    QtImGui::newFrame();
    mAircraft->DrawGui();
    mImGuiWidget->Draw();
    ImGui::Render();
    QtImGui::render();
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
