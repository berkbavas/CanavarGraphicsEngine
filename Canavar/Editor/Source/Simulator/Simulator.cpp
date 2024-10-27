#include "Simulator.h"

void Simulator::Initialize(Canavar::Engine::NodePtr pRootNode, Canavar::Engine::ModelPtr pJetModel)
{
    mAircraft = new Aircraft;
    mAircraftController = new AircraftController(mAircraft);

    mAircraftController->SetRootNode(pRootNode);
    mAircraftController->SetJetNode(pJetModel);

    mAircraftController->Initialize();
    mAircraft->Initialize();
}

void Simulator::Update(float ifps)
{
    mAircraftController->Update(ifps);
}

void Simulator::KeyPressed(QKeyEvent* pEvent)
{
    mAircraftController->KeyPressed(pEvent);
}

void Simulator::KeyReleased(QKeyEvent* pEvent)
{
    mAircraftController->KeyReleased(pEvent);
}
