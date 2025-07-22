#include "Viewer.h"

#include <Canavar/Engine/Core/Window.h>
#include <imgui.h>

Canavar::Viewer::Viewer::Viewer()
{
    mController = new Canavar::Engine::Controller(Canavar::Engine::ContainerMode::Widget);
    mController->AddEventReceiver(this);

    mOpenGLWidget = mController->GetWidget();
}

void Canavar::Viewer::Viewer::Run()
{
    mOpenGLWidget->showMaximized();
}

void Canavar::Viewer::Viewer::Initialize()
{
    initializeOpenGLFunctions();

    mRenderRef = QtImGui::initialize(mOpenGLWidget);

    mNodeManager = mController->GetNodeManager();
    mCameraManager = mController->GetCameraManager();
    mPersecutorCamera = std::make_shared<Canavar::Engine::PersecutorCamera>();

    // Post initialize
    mNodeManager->ImportNodes("Resources/World.json");
    mFreeCamera = mCameraManager->GetFreeCamera();
    mNode = mNodeManager->FindNodeByName<Canavar::Engine::DummyObject>("DummyObject");
    mNode->RotateGlobal(QVector3D(0, 1, 0), 90);
    mCameraManager->SetActiveCamera(mPersecutorCamera);
    mPersecutorCamera->SetTarget(mNode);
}

void Canavar::Viewer::Viewer::PostRender(float ifps)
{
    glDisable(GL_CULL_FACE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    QtImGui::newFrame();

    ImGui::Begin("Debug");

    if (ImGui::Button("Up"))
    {
        mPersecutorCamera->AnimateTo(Engine::ViewDirection::Up);
    }

    if (ImGui::Button("Down"))
    {
        mPersecutorCamera->AnimateTo(Engine::ViewDirection::Down);
    }

    if (ImGui::Button("Front"))
    {
        mPersecutorCamera->AnimateTo(Engine::ViewDirection::Front);
    }

    if (ImGui::Button("Back"))
    {
        mPersecutorCamera->AnimateTo(Engine::ViewDirection::Back);
    }

    if (ImGui::Button("Left"))
    {
        mPersecutorCamera->AnimateTo(Engine::ViewDirection::Left);
    }

    if (ImGui::Button("Right"))
    {
        mPersecutorCamera->AnimateTo(Engine::ViewDirection::Right);
    }

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

    ImGui::Render();
    QtImGui::render();
}

bool Canavar::Viewer::Viewer::KeyPressed(QKeyEvent* pEvent)
{
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