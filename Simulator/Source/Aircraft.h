#pragma once

#include "Converter.h"

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QVariant>

#include <QQuaternion>

namespace JSBSim
{
    class FGFDMExec;
    class FGFCS;
    class FGPropagate;
    class FGPropulsion;
    class FGAuxiliary;
}

class Aircraft : public QObject
{
    Q_OBJECT
public:
    enum class Command {
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

    struct PrimaryFlightData {
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
        QVector3D position;
        QQuaternion rotation;
        double rudderPos;
        double elevatorPos;
        double leftAileronPos;
        double rightAileronPos;
    };

    explicit Aircraft(QObject* parent = nullptr);

    bool Init();
    bool GetHolding() const;

public slots:
    void OnCommand(Aircraft::Command command, QVariant variant = QVariant());
    void Tick();
    void Stop();

signals:
    void PfdChanged(Aircraft::PrimaryFlightData);

private:
    JSBSim::FGFDMExec* mExecutor;
    JSBSim::FGFCS* mCommander;
    JSBSim::FGPropagate* mPropagate;
    JSBSim::FGPropulsion* mPropulsion;
    JSBSim::FGAuxiliary* mAuxiliary;
    Converter* mConverter;

    QThread mThread;
    QTimer mTimer;
};

Q_DECLARE_METATYPE(Aircraft::Command);
Q_DECLARE_METATYPE(Aircraft::PrimaryFlightData);
