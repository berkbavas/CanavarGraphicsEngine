#include "Simulator.h"

#include "Canavar/Engine/Core/OpenGLWidget.h"
#include "Canavar/Engine/Manager/CameraManager.h"
#include "Canavar/Engine/Manager/NodeManager.h"
#include "Canavar/Engine/Manager/Renderer.h"
#include "Canavar/Engine/Util/ImGuiWidget.h"

#include <imgui.h>

Canavar::Simulator::Simulator::Simulator()
{
    mWidget = new Canavar::Engine::OpenGLWidget(nullptr);

    mRenderer = new Canavar::Engine::Renderer(mWidget);

    connect(mRenderer, &Canavar::Engine::Renderer::Initialized, this, &Simulator::Initialize);
    connect(mRenderer, &Canavar::Engine::Renderer::Updated, this, &Simulator::Update);
    connect(mRenderer, &Canavar::Engine::Renderer::PostRender, this, &Simulator::OnPostRender);
    mRenderer->AddEventReceiver(this);
    mAircraft = new Aircraft;
    mImGuiWidget = new Canavar::Engine::ImGuiWidget(mRenderer);
}

Canavar::Simulator::Simulator::~Simulator()
{
    LOG_DEBUG("Simulator::~Simulator: Destructor called");

    // Order of deletion is important here.
    // The Renderer should be deleted before the OpenGLWidget to ensure that all OpenGL resources are released properly.
    mWidget->MakeCurrent();

    delete mImGuiWidget;
    delete mAircraft;
    delete mRenderer;
    delete mWidget;
}

void Canavar::Simulator::Simulator::Run()
{
    if (mAircraft->Initialize() == false)
    {
        CGE_EXIT_FAILURE("Simulator::Run: Aircraft initialization failed. Exiting...");
    }

    mWidget->showMinimized();
    mWidget->showMaximized();
}

void Canavar::Simulator::Simulator::Initialize()
{
    mRenderRef = QtImGui::initialize(mWidget, false);

    mNodeManager = mRenderer->GetNodeManager();
    mCameraManager = mRenderer->GetCameraManager();

    mNodeManager->ImportNodes("Resources/f16.json");

    mFreeCamera = mNodeManager->FindNodeByType<Canavar::Engine::FreeCamera>();
    mDummyCamera = mNodeManager->FindNodeByType<Canavar::Engine::DummyCamera>();
    mPersecutorCamera = mNodeManager->FindNodeByType<Canavar::Engine::PersecutorCamera>();

    mRootNode = mNodeManager->FindNodeByName<Canavar::Engine::DummyObject>("Root Node");
    mJetNode = mNodeManager->FindNodeByName<Canavar::Engine::TexturedModel>("f16");

    mCameraManager->SetActiveCamera(mPersecutorCamera);
    mPersecutorCamera->SetTarget(mRootNode);
}

void Canavar::Simulator::Simulator::Update(float ifps)
{
    mAircraft->Tick(ifps);
}

void Canavar::Simulator::Simulator::OnPostRender(float Ifps)
{
    const auto& Pfd = mAircraft->GetPfd();

    // Update the root node's position and rotation based on the aircraft's PFD data
    if (mRootNode)
    {
        mRootNode->SetPosition(Pfd.Position);
        mRootNode->SetRotation(Pfd.Rotation);
    }

    DrawImGui();
}

void Canavar::Simulator::Simulator::DrawImGui()
{
    QtImGui::newFrame(mRenderRef);
    mAircraft->DrawGui();
    ImGui::Render();
    QtImGui::render(mRenderRef);
}

bool Canavar::Simulator::Simulator::OnKeyPressed(QKeyEvent* pEvent)
{
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

    return mAircraft->OnKeyPressed(pEvent);
}

bool Canavar::Simulator::Simulator::OnKeyReleased(QKeyEvent* pEvent)
{
    return mAircraft->OnKeyReleased(pEvent);
}
