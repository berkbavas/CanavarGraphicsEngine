#include "Simulator.h"

#include "Canavar/Engine/Camera/FreeCamera.h"
#include "Canavar/Engine/Camera/GlobeCamera.h"
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

    connect(mImGuiWidget, &Canavar::Engine::ImGuiWidget::CanDrawImGuiWidgets, this, &Simulator::DrawImGui);
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
    mNodeManager = mRenderer->GetNodeManager();
    mCameraManager = mRenderer->GetCameraManager();

    mNodeManager->ImportNodes("Resources/f16.json");

    mFreeCamera = mNodeManager->FindNodeByType<Canavar::Engine::FreeCamera>();
    mGlobeCamera = mNodeManager->CreateNode<Canavar::Engine::GlobeCamera>();
    mDummyCamera = mNodeManager->FindNodeByType<Canavar::Engine::DummyCamera>();
    mPersecutorCamera = mNodeManager->FindNodeByType<Canavar::Engine::PersecutorCamera>();

    mRootNode = mNodeManager->FindNodeByName<Canavar::Engine::DummyObject>("Root Node");
    mJetNode = mNodeManager->FindNodeByName<Canavar::Engine::TexturedModel>("f16");

    mCameraManager->SetActiveCamera(mPersecutorCamera);
    mPersecutorCamera->SetTarget(mRootNode);

    QOpenGLFramebufferObjectFormat Format;
    Format.setAttachment(QOpenGLFramebufferObject::Depth);
    Format.setSamples(0);
    Format.setInternalTextureFormat(GL_RGBA32F);
    mAircraftCameraFramebuffer = std::make_unique<Canavar::Engine::Framebuffer>(512, 512, Format);

    mAircraftCamera = mNodeManager->CreateNode<Canavar::Engine::DummyCamera>();
    mAircraftCamera->Resize(512, 512);
}

void Canavar::Simulator::Simulator::Update(float Ifps)
{
    mAircraft->Tick(Ifps);

    if (mDummyCamera)
    {
        mAircraftCamera->SetPosition(mDummyCamera->GetWorldPosition());
        mAircraftCamera->SetRotation(mDummyCamera->GetWorldRotation());
    }

    mRenderer->RenderToFramebuffer(mAircraftCameraFramebuffer.get(), mAircraftCamera);
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
}

void Canavar::Simulator::Simulator::DrawImGui(float Ifps)
{
    mAircraft->DrawGui();

    DrawAircraftCameraFramebuffer(Ifps);
}

void Canavar::Simulator::Simulator::DrawAircraftCameraFramebuffer(float Ifps)
{
    ImGui::Begin("Aircraft Camera View");
    const auto ImageSize = ImVec2(mAircraftCameraFramebuffer->GetWidth(), mAircraftCameraFramebuffer->GetHeight());
    ImGui::Image(ImTextureID(mAircraftCameraFramebuffer->GetTexture()), ImageSize, ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();
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
    else if (pEvent->key() == Qt::Key_4)
    {
        mCameraManager->SetActiveCamera(mGlobeCamera);
        return true;
    }

    return mAircraft->OnKeyPressed(pEvent);
}

bool Canavar::Simulator::Simulator::OnKeyReleased(QKeyEvent* pEvent)
{
    return mAircraft->OnKeyReleased(pEvent);
}

bool Canavar::Simulator::Simulator::OnLeaveEvent(QEvent* pEvent)
{
    mAircraft->OnLeaveEvent(pEvent);
    return false;
}
