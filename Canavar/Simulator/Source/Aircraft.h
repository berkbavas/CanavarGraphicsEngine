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
            ChangeSmoothPositionCoefficient,
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
            QVector3D actualPosition;
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

        qint64 mCurrentTime{ 0 };
        qint64 mPreviousTime{ 0 };

        QVector3D mPreviousPosition{ 0.0f, 0.0f, 0.0f };
        QVector3D mCurrentPosition{ 0.0f, 0.0f, 0.0f };

        float mSmoothPositionCoefficient{ 10.0f }; // Multiplier for smoothing position updates

        static constexpr float FEET_TO_METER{ 0.3048f }; // 1 foot = 0.3048 meters
        static constexpr float METER_TO_FEET{ 1 / FEET_TO_METER };
    };

    Q_DECLARE_METATYPE(Aircraft::Command);
    Q_DECLARE_METATYPE(Aircraft::PrimaryFlightData);
}
