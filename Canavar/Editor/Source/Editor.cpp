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
    mCameraManager = mController->GetCameraManager();
    mRenderingManager = mController->GetRenderingManager();

    mImGuiWidget->SetRenderingManager(mRenderingManager);
    mImGuiWidget->SetNodeManager(mNodeManager);
    mImGuiWidget->SetCameraManager(mCameraManager);
    mImGuiWidget->SetVertexPainter(mController->GetVertexPainter());
    mImGuiWidget->Initialize();

    mFreeCamera = mCameraManager->GetFreeCamera();

    connect(mImGuiWidget, &ImGuiWidget::GoToObject, this, [=](Engine::ObjectPtr pObject) {
        mFreeCamera->GoToObject(pObject);
        mCameraManager->SetActiveCamera(mFreeCamera);
    });

    mNodeManager->GetTerrain()->SetEnabled(false);

    mNodeManager->ImportNodes("Resources/CrossSection.json");

    // LightningStrikeGeneratorPtr pGenerator = std::make_shared<LightningStrikeGenerator>();
    // pGenerator->SetPosition(0, 0, -10);

    // LightningStrikeAttractorPtr pAttractor0 = std::make_shared<LightningStrikeAttractor>();
    // pAttractor0->SetPosition(10, 0, -10);

    // LightningStrikeAttractorPtr pAttractor1 = std::make_shared<LightningStrikeAttractor>();
    // pAttractor1->SetPosition(5, 5, -10);

    // pGenerator->AddAttractor(pAttractor0);
    // pGenerator->AddAttractor(pAttractor1);

    // mNodeManager->AddNode(pGenerator);
    // mNodeManager->AddNode(pAttractor0);
    // mNodeManager->AddNode(pAttractor1);
    // mNodeManager->GetSun()->SetDirectionFromThetaPhi(0, 45);
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
