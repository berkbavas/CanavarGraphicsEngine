#pragma once

#include "Aircraft.h"

#include <imgui.h>

#include <QKeyEvent>
#include <QObject>
#include <QSet>
#include <QThread>
#include <QTimer>
#include <QtImGui.h>

namespace Canavar::Simulator
{
    class AircraftController : public QObject
    {
        Q_OBJECT
      public:
        explicit AircraftController(Aircraft* aircraft, QObject* parent = nullptr);

        bool Initialize();
        void DrawGui();
        void KeyPressed(QKeyEvent*);
        void KeyReleased(QKeyEvent*);
        const Aircraft::PrimaryFlightData& GetPrimaryFlightData() const;

      signals:
        void Command(Aircraft::Command command, QVariant variant = QVariant());

      private:
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

        bool mAutoPilotEnabled{ false };

        float mSmoothPositionCoefficent{ 10.0f }; // Multiplier for smoothing position updates
    };
}