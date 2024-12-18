#pragma once

#include "Converter.h"

#include <QObject>
#include <QQuaternion>
#include <QTimer>
#include <QVariant>

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
    class Aircraft : public QObject
    {
        Q_OBJECT
      public:
        explicit Aircraft(QObject* parent = nullptr);

        bool Initialize();
        bool IsHolding() const;

        enum class Command
        {
            Aileron,
            Elevator,
            Rudder,
            Steering,
            Flaps,
            Speedbrake,
            Spoiler,
            PitchTrim,
            RudderTrim,
            AileronTrim,
            Throttle,
            Mixture,
            Gear,
            PropellerPitch,
            PropellerFeather,
            InitRunning,
            Hold,
            Resume,
        };
        Q_ENUM(Command);

        struct PrimaryFlightData
        {
            double angleOfAttack;
            double sideSlip;
            double roll;
            double pitch;
            double heading;
            double airspeed;
            double machNumber;
            double climbRate;
            double latitude;
            double longitude;
            double altitude;
            double pressure;
            double turnRate;
            double slipSkid;
            QVector3D position;
            QQuaternion rotation;
            double rudderPos;
            double elevatorPos;
            double leftAileronPos;
            double rightAileronPos;
        };

      public slots:
        void ProcessCommand(Aircraft::Command command, QVariant variant = QVariant());
        void Tick();

      signals:
        void PfdChanged(Aircraft::PrimaryFlightData);

      private:
        JSBSim::FGFDMExec* mExecutor;
        JSBSim::FGFCS* mCommander;
        JSBSim::FGPropagate* mPropagate;
        JSBSim::FGPropulsion* mPropulsion;
        JSBSim::FGAuxiliary* mAuxiliary;

        Converter* mConverter;

        PrimaryFlightData mPfd;
    };

    Q_DECLARE_METATYPE(Aircraft::Command);
    Q_DECLARE_METATYPE(Aircraft::PrimaryFlightData);
}
