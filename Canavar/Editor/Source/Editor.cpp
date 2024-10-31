#include "Editor.h"

#include "Canavar/Engine/Core/EventReceiver.h"
#include "Canavar/Engine/Core/Window.h"
#include "Canavar/Engine/Manager/CameraManager.h"
#include "Canavar/Engine/Manager/LightManager.h"
#include "Canavar/Engine/Manager/NodeManager.h"
#include "Canavar/Engine/Manager/RenderingManager/RenderingManager.h"
#include "Canavar/Engine/Manager/ShaderManager.h"
#include "Canavar/Engine/Node/Object/DummyObject/DummyObject.h"
#include "Canavar/Engine/Node/Object/Light/PointLight.h"
#include "Canavar/Engine/Node/Object/LightningStrike/LightningStrikeAttractor.h"
#include "Canavar/Engine/Node/Object/LightningStrike/LightningStrikeGenerator.h"
#include "Canavar/Engine/Node/Object/Model/Model.h"
#include "Canavar/Engine/Util/Logger.h"

#include <imgui.h>

#include <QtImGui.h>

using namespace Canavar::Engine;

Canavar::Editor::Editor::Editor()
{
    mController = new Controller;
    mController->AddEventReceiver(this);

    mImGuiWidget = new ImGuiWidget;
    mSimulator = new Simulator;
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
    mImGuiWidget->Initialize();

    mFreeCamera = mCameraManager->GetFreeCamera();

    mPersecutorCamera = std::make_shared<PersecutorCamera>();

    connect(mImGuiWidget, &ImGuiWidget::GoToObject, this, [=](Engine::ObjectPtr pObject) {
        mFreeCamera->GoToObject(pObject);
        mCameraManager->SetActiveCamera(mFreeCamera);
    });

    CreateSimulatorModels();

    Test();
}

void Canavar::Editor::Editor::Test()
{
    ModelPtr pCustomModel0 = std::make_shared<Model>("Cyborg");
    pCustomModel0->SetNodeName("Cyborg 1");
    pCustomModel0->SetPosition(0, 3, 3);

    ModelPtr pCustomModel1 = std::make_shared<Model>("Cyborg");
    pCustomModel1->SetNodeName("Cyborg 2");
    pCustomModel1->SetPosition(0, 3, -3);
    mNodeManager->AddNode(pCustomModel1);
    mNodeManager->AddNode(pCustomModel0);

    LightningStrikeAttractorPtr pAttractor = std::make_shared<LightningStrikeAttractor>();
    pAttractor->SetPosition(0, 10, 0);

    LightningStrikeGeneratorPtr pGenerator = std::make_shared<LightningStrikeGenerator>();
    pGenerator->AddAttractor(pAttractor);

    mNodeManager->AddNode(pAttractor);
    mNodeManager->AddNode(pGenerator);

    ModelPtr pCrossfire = std::make_shared<Model>("Crossfire");
    pCrossfire->SetAmbient(0.8f);
    pCrossfire->SetRoll(179.999);
    pCrossfire->SetWorldPosition(0, 2060, 0);
    mNodeManager->AddNode(pCrossfire);
}

void Canavar::Editor::Editor::CreateSimulatorModels()
{
    // Root
    DummyObjectPtr pRootNode = std::make_shared<DummyObject>();
    pRootNode->SetNodeName("Root Node");

    mPersecutorCamera->SetTarget(pRootNode);

    // F16
    ModelPtr pModel = std::make_shared<Model>("f16c");
    pModel->SetInvertNormals(true);
    pModel->SetNodeName("f16c");
    pRootNode->AddChild(pModel);

    // Red light
    {
        ModelPtr pSphere = std::make_shared<Model>("Sphere");
        pSphere->SetScale(0.5f, 0.5f, 0.5f);

        pSphere->SetColor(QVector4D(1, 0, 0, 1));

        PointLightPtr pRedLight = std::make_shared<PointLight>();
        pRedLight->SetColor(QVector4D(1, 0, 0, 1));
        pRedLight->SetConstant(0.075f);
        pRedLight->SetQuadratic(0.1f);
        pRedLight->SetLinear(0.25f);
        pRedLight->SetNodeName("Red Light");

        pSphere->AddChild(pRedLight);
        pRootNode->AddChild(pSphere);
        pSphere->SetPosition(-11.42f, 0.16f, -0.83f);
    }

    // Green light
    {
        ModelPtr pSphere = std::make_shared<Model>("Sphere");
        pSphere->SetScale(0.5f, 0.5f, 0.5f);
        pSphere->SetColor(QVector4D(0, 1, 0, 1));

        PointLightPtr pGreenLight = std::make_shared<PointLight>();
        pGreenLight->SetColor(QVector4D(0, 1, 0, 1));
        pGreenLight->SetConstant(0.075f);
        pGreenLight->SetQuadratic(0.1f);
        pGreenLight->SetLinear(0.25f);
        pGreenLight->SetNodeName("Green Light");

        pSphere->AddChild(pGreenLight);
        pRootNode->AddChild(pSphere);
        pSphere->SetPosition(11.42f, 0.16f, -0.83f);
    }

    // Nozzle Effect
    {
        NozzleEffectPtr pNozzleEffect = std::make_shared<NozzleEffect>();
        pNozzleEffect->SetNodeName("Nozzle Effect");
        pRootNode->AddChild(pNozzleEffect);
        pNozzleEffect->SetPosition(0.0f, 0.14f, 11.5f);
    }

    // Dummy Camera
    {
        mDummyCamera = std::make_shared<DummyCamera>();
        pRootNode->AddChild(mDummyCamera);
        mDummyCamera->SetPosition(0, 7.5f, 60);
    }

    mNodeManager->AddNode(pRootNode);
    pRootNode->SetWorldPosition(0, 20, 0);

    mSimulator->Initialize(pRootNode, pModel);
}

void Canavar::Editor::Editor::Update(float ifps)
{
    QtImGui::newFrame();

    mImGuiWidget->Draw();
    mSimulator->Update(ifps);

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

    if (pEvent->key() == Qt::Key_1)
    {
        mFreeCamera->SetWorldPosition(mPersecutorCamera->GetWorldPosition());
        mCameraManager->SetActiveCamera(mFreeCamera);
    }
    else if (pEvent->key() == Qt::Key_2)
    {
        mCameraManager->SetActiveCamera(mPersecutorCamera);
    }
    else if (pEvent->key() == Qt::Key_3)
    {
        mCameraManager->SetActiveCamera(mDummyCamera);
    }
    else
    {
        mSimulator->KeyPressed(pEvent);
    }

    return mImGuiWidget->KeyPressed(pEvent);
}

bool Canavar::Editor::Editor::KeyReleased(QKeyEvent *pEvent)
{
    mSimulator->KeyReleased(pEvent);

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
