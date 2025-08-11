#pragma once

#include "Canavar/Engine/Util/Logger.h"
#include "Command.h"
#include "Converter.h"
#include "PrimaryFlightData.h"

#include <imgui.h>

#include <QKeyEvent>
#include <QMatrix3x3>
#include <QObject>
#include <QSet>
#include <QVariant>
#include <QtImGui.h>

namespace JSBSim
{
    class FGFDMExec;
    class FGFCS;
    class FGPropagate;
    class FGPropulsion;
    class FGAuxiliary;
}

namespace Canavar::Simulator
{
    class Aircraft
    {
      public:
        Aircraft() = default;

        ~Aircraft();

        bool Initialize();
        void DrawGui();
        void OnKeyPressed(QKeyEvent* pEvent);
        void OnKeyReleased(QKeyEvent* pEvent);
        void Tick(float ifps);
        bool IsHolding() const;

        const PrimaryFlightData& GetPfd() const;

      private:
        void ProcessInputs();
        void ProcessAutoPilotIfEnabled();
        void Run(float ifps);
        void ProcessCommand(Command command, QVariant variant = QVariant());

        QVariant GetAutoPilotCommand(Command command, QVariant value);

        JSBSim::FGFDMExec* mExecutor{ nullptr };
        JSBSim::FGFCS* mCommander{ nullptr };
        JSBSim::FGPropagate* mPropagate{ nullptr };
        JSBSim::FGPropulsion* mPropulsion{ nullptr };
        JSBSim::FGAuxiliary* mAuxiliary{ nullptr };

        PrimaryFlightData mPfd;
        Converter* mConverter{ nullptr };

        QSet<Qt::Key> mPressedKeys;

        float mAileron{ 0.0f };  // [-1, 1]
        float mElevator{ 0.0f }; // [-1, 1]
        float mRudder{ 0.0f };   // [-1, 1]
        float mThrottle{ 0.0f }; // [0, 1]

        bool mAutoPilotEnabled{ false };

        double mRefLatitude{ 0.0 };
        double mRefLongitude{ 0.0 };
        double mRefAltitude{ 0.0 };
    };
}
