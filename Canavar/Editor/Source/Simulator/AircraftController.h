#pragma once

#include "Aircraft.h"

#include <Canavar/Engine/Node/Model/Model.h>
#include <imgui.h>

#include <QKeyEvent>
#include <QObject>
#include <QSet>
#include <QThread>
#include <QTimer>
#include <QtImGui.h>

class AircraftController : public QObject
{
    Q_OBJECT
  public:
    explicit AircraftController(Aircraft* aircraft, QObject* parent = nullptr);

    bool Initialize();
    void Update(float ifps);

    void KeyPressed(QKeyEvent*);
    void KeyReleased(QKeyEvent*);

    void SetJetNode(Canavar::Engine::ModelPtr pJet);
    void SetRootNode(Canavar::Engine::NodePtr pRoot);

  signals:
    void Command(Aircraft::Command command, QVariant variant = QVariant());

  private:
    void DrawImGui();
    void Tick();
    QVariant GetCmd(Aircraft::Command command, QVariant value);

  private:
    Aircraft* mAircraft;
    QSet<Qt::Key> mPressedKeys;
    QTimer mTimer;

    float mAileron;  // [-1, 1]
    float mElevator; // [-1, 1]
    float mRudder;   // [-1, 1]
    float mThrottle; // [0, 1]

    Aircraft::PrimaryFlightData mPfd;

    Canavar::Engine::ModelPtr mJetNode;
    Canavar::Engine::NodePtr mRootNode;

    bool mAutoPilotEnabled{ false };

    float mTimeElapsed;
};