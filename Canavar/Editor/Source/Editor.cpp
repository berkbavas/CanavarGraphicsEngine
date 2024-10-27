#include "Editor.h"

#include "Canavar/Engine/Core/EventReceiver.h"
#include "Canavar/Engine/Core/Window.h"
#include "Canavar/Engine/Manager/CameraManager.h"
#include "Canavar/Engine/Manager/LightManager.h"
#include "Canavar/Engine/Manager/NodeManager.h"
#include "Canavar/Engine/Manager/RenderingManager.h"
#include "Canavar/Engine/Manager/ShaderManager.h"
#include "Canavar/Engine/Node/Light/PointLight.h"
#include "Canavar/Engine/Node/Model/Model.h"
#include "Canavar/Engine/Util/Logger.h"

#include <imgui.h>

#include <QtImGui.h>

using namespace Canavar::Engine;

Canavar::Editor::Editor::Editor()
{
    mController = new Controller;
    mController->AddEventReceiver(this);

    mImGuiWidget = new ImGuiWidget;
}

void Canavar::Editor::Editor::Run()
{
    mController->Run();
}

void Canavar::Editor::Editor::Initialize()
{
    QtImGui::initialize(mController->GetWindow());

    mNodeManager = mController->GetNodeManager();
    mImGuiWidget->SetNodeManager(mNodeManager);
    mImGuiWidget->Initialize();

    ModelPtr pModel = std::make_shared<Model>("JetFighter");
    mNodeManager->AddNode(pModel);

    PointLightPtr pPointLight = std::make_shared<PointLight>();
    pPointLight->SetPosition(0, 3, 0);
    pPointLight->SetColor(QVector4D(1, 0, 0, 1));
    mNodeManager->AddNode(pPointLight);
}

void Canavar::Editor::Editor::Update(float ifps)
{
    QtImGui::newFrame();

    mImGuiWidget->DrawWidget();

    ImGui::Render();
    QtImGui::render();
}

bool Canavar::Editor::Editor::KeyPressed(QKeyEvent *pEvent)
{
    if (ImGui::GetIO().WantCaptureKeyboard)
    {
        return true;
    }

    return false;
}

bool Canavar::Editor::Editor::KeyReleased(QKeyEvent *pEvent)
{
    if (ImGui::GetIO().WantCaptureKeyboard)
    {
        return true;
    }

    return false;
}

bool Canavar::Editor::Editor::MousePressed(QMouseEvent *pEvent)
{
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return true;
    }

    return false;
}

bool Canavar::Editor::Editor::MouseReleased(QMouseEvent *pEvent)
{
    return false;
}

bool Canavar::Editor::Editor::MouseMoved(QMouseEvent *pEvent)
{
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return true;
    }

    return false;
}

bool Canavar::Editor::Editor::WheelMoved(QWheelEvent *pEvent)
{
    return false;
}
