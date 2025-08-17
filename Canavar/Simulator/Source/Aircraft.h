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

        std::shared_ptr<JSBSim::FGFDMExec> mExecutor{ nullptr };
        std::shared_ptr<JSBSim::FGFCS> mCommander{ nullptr };
        std::shared_ptr<JSBSim::FGPropagate> mPropagate{ nullptr };
        std::shared_ptr<JSBSim::FGPropulsion> mPropulsion{ nullptr };
        std::shared_ptr<JSBSim::FGAuxiliary> mAuxiliary{ nullptr };

        std::unique_ptr<Converter> mConverter{ nullptr };

        PrimaryFlightData mPfd;

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
