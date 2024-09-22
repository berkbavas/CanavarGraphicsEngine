#pragma once

#include "Aircraft.h"

#include <Model.h>

#include <PointLight.h>
#include <QKeyEvent>
#include <QObject>
#include <QSet>
#include <QTimer>

#include <imgui.h>
#include <QtImGui.h>

class AircraftController : public QObject
{
    Q_OBJECT
public:
    explicit AircraftController(Aircraft* aircraft, QObject* parent = nullptr);

    void KeyPressed(QKeyEvent*);
    void KeyReleased(QKeyEvent*);

    void DrawGUI();
    void SetJet(Canavar::Engine::Model* newJet);
    void SetRootJetNode(Canavar::Engine::Node* newRootJetNode);

signals:
    void Command(Aircraft::Command command, QVariant variant = QVariant());

public slots:
    bool Init();
    void Tick();
    void Update(float ifps);

private:
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

    Canavar::Engine::Model* mJet;
    Canavar::Engine::Node* mRootJetNode;
    Canavar::Engine::PointLight* mRedLight;
    Canavar::Engine::PointLight* mGreenLight;

    bool mAutoPilotEnabled;

    float mTimeElapsed;
};