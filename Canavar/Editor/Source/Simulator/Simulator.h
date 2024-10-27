#pragma once

#include "Simulator/Aircraft.h"
#include "Simulator/AircraftController.h"

#include <Canavar/Engine/Node/Model/Model.h>

class Simulator
{
  public:
    Simulator() = default;

    void Initialize(Canavar::Engine::NodePtr pRootNode, Canavar::Engine::ModelPtr pJetModel);

    void Update(float ifps);

    void KeyPressed(QKeyEvent *pEvent);
    void KeyReleased(QKeyEvent *pEvent);

  private:
    Aircraft *mAircraft;
    AircraftController *mAircraftController;
};