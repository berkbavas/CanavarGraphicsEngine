#include "Editor.h"

#include "Canavar/Engine/Core/EventReceiver.h"
#include "Canavar/Engine/Core/Window.h"
#include "Canavar/Engine/Manager/CameraManager.h"
#include "Canavar/Engine/Manager/LightManager.h"
#include "Canavar/Engine/Manager/NodeManager.h"
#include "Canavar/Engine/Manager/RenderingManager/RenderingManager.h"
#include "Canavar/Engine/Manager/ShaderManager.h"
#include "Canavar/Engine/Node/Object/Light/PointLight.h"
#include "Canavar/Engine/Node/Object/LightningStrike/LightningStrikeAttractor.h"
#include "Canavar/Engine/Node/Object/LightningStrike/LightningStrikeGenerator.h"
#include "Canavar/Engine/Util/Logger.h"

#include <imgui.h>

#include <QtImGui.h>

using namespace Canavar::Engine;

Canavar::Editor::Editor::Editor(QObject *pParent)
    : QObject(pParent)
{
    mWindow = new Engine::Window(nullptr);
    mController = new Controller(mWindow, this);
    mController->AddEventReceiver(this);

    mImGuiWidget = new ImGuiWidget(this);
}

Canavar::Editor::Editor::~Editor()
{
    LOG_DEBUG("Editor::~Editor: Destructor called");
}

void Canavar::Editor::Editor::Run()
{
    mWindow->showMinimized();
}

void Canavar::Editor::Editor::Initialize()
{
    mWindow->showMaximized();

    QtImGui::initialize(mWindow);

    mNodeManager = mController->GetNodeManager();
    mCameraManager = mController->GetCameraManager();
    mRenderingManager = mController->GetRenderingManager();

    mImGuiWidget->SetRenderingManager(mRenderingManager);
    mImGuiWidget->SetNodeManager(mNodeManager);
    mImGuiWidget->SetCameraManager(mCameraManager);
    mImGuiWidget->Initialize();

    mFreeCamera = mCameraManager->GetFreeCamera();

    connect(mImGuiWidget, &ImGuiWidget::GoToObject, this, [=](Engine::ObjectPtr pObject) {
        mFreeCamera->GoToObject(pObject);
        mCameraManager->SetActiveCamera(mFreeCamera);
    });

    mNodeManager->ImportNodes("Resources/Empty.json");
}

void Canavar::Editor::Editor::PostRender(float ifps)
{
    QtImGui::newFrame();

    mImGuiWidget->Draw();

    ImGui::Render();
    QtImGui::render();
}

void Canavar::Editor::Editor::Render(float ifps) {}

bool Canavar::Editor::Editor::KeyPressed(QKeyEvent *pEvent)
{
    if (ImGui::GetIO().WantCaptureKeyboard)
    {
        return true;
    }

    return mImGuiWidget->KeyPressed(pEvent);
}

bool Canavar::Editor::Editor::KeyReleased(QKeyEvent *pEvent)
{
    return mImGuiWidget->KeyReleased(pEvent);
}

bool Canavar::Editor::Editor::MousePressed(QMouseEvent *pEvent)
{
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return true;
    }

    return mImGuiWidget->MousePressed(pEvent);
}

bool Canavar::Editor::Editor::MouseReleased(QMouseEvent *pEvent)
{
    return mImGuiWidget->MouseReleased(pEvent);
}

bool Canavar::Editor::Editor::MouseMoved(QMouseEvent *pEvent)
{
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return true;
    }

    return mImGuiWidget->MouseMoved(pEvent);
}

bool Canavar::Editor::Editor::WheelMoved(QWheelEvent *pEvent)
{
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return true;
    }

    return false;
}
