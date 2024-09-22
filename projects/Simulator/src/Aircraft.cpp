#include "Aircraft.h"

#include <QDebug>
#include <QDir>
#include <QtMath>

#include <FGFDMExec.h>
#include <FGJSBBase.h>
#include <initialization/FGInitialCondition.h>
#include <models/FGAuxiliary.h>
#include <models/FGFCS.h>
#include <models/FGPropagate.h>
#include <models/FGPropulsion.h>
#include <models/propulsion/FGThruster.h>
#include <math/FGQuaternion.h>

Aircraft::Aircraft(QObject* parent)
    : QObject(parent)
{}

bool Aircraft::Init()
{
    QString currentPath = QDir::currentPath();
    QDir::setCurrent("resources/Data");

    mExecutor = new JSBSim::FGFDMExec;
    mExecutor->SetDebugLevel(0);
    mExecutor->DisableOutput();

    qInfo() << "Aircraft is being initializing...";

    if (!mExecutor->LoadModel("f16"))
    {
        qCritical() << "Could not load JSBSim model.";
        return false;
    }

    if (!mExecutor->GetIC()->Load(SGPath("reset00.xml")))
    {
        qCritical() << "Could not load JSBSim initial conditions.";
        return false;
    }

    if (!mExecutor->RunIC())
    {
        qCritical() << "Could not run JSBSim initial conditions.";
        return false;
    }

    mCommander = mExecutor->GetFCS();
    mPropagate = mExecutor->GetPropagate();
    mPropulsion = mExecutor->GetPropulsion();
    mAuxiliary = mExecutor->GetAuxiliary();
    mExecutor->Setdt(0.01);

    double latitude = mExecutor->GetIC()->GetLatitudeDegIC();
    double longitude = mExecutor->GetIC()->GetLongitudeDegIC();

    mConverter = new Converter(latitude, longitude, 0.0);

    moveToThread(&mThread);
    mThread.start();

    connect(&mTimer, &QTimer::timeout, this, &Aircraft::Tick, Qt::QueuedConnection);
    mTimer.start(10);

    QDir::setCurrent(currentPath);

    qInfo() << "Aircraft is initialized.";

    return true;
}

void Aircraft::OnCommand(Aircraft::Command command, QVariant variant)
{
    switch (command)
    {
    case Command::InitRunning:
        mPropulsion->InitRunning(variant.toInt());
        break;
    case Command::Hold:
        mExecutor->Hold();
        break;
    case Command::Resume:
        mExecutor->Resume();
        break;
    case Command::Aileron:
        mCommander->SetDaCmd(variant.toDouble());
        break;
    case Command::Elevator:
        mCommander->SetDeCmd(variant.toDouble());
        break;
    case Command::Rudder:
        mCommander->SetDrCmd(variant.toDouble());
        break;
    case Command::Steering:
        mCommander->SetDsCmd(variant.toDouble());
        break;
    case Command::Flaps:
        mCommander->SetDfCmd(variant.toDouble());
        break;
    case Command::Speedbrake:
        mCommander->SetDsbCmd(variant.toDouble());
        break;
    case Command::Spoiler:
        mCommander->SetDspCmd(variant.toDouble());
        break;
    case Command::PitchTrim:
        mCommander->SetPitchTrimCmd(variant.toDouble());
        break;
    case Command::RudderTrim:
        mCommander->SetYawTrimCmd(variant.toDouble());
        break;
    case Command::AileronTrim:
        mCommander->SetRollTrimCmd(variant.toDouble());
        break;
    case Command::Throttle:
        mCommander->SetThrottleCmd(-1, variant.toDouble());
        break;
    case Command::Mixture:
        mCommander->SetMixtureCmd(-1, variant.toDouble());
        break;
    case Command::Gear:
        mCommander->SetGearCmd(variant.toDouble());
        break;
    case Command::PropellerPitch:
        mCommander->SetPropAdvanceCmd(-1, variant.toDouble());
        break;
    case Command::PropellerFeather:
        mCommander->SetFeatherCmd(-1, variant.toDouble());
        break;
    }
}

void Aircraft::Tick()
{
    mPropagate->SetTerrainElevation(0);

    mExecutor->Run();

    PrimaryFlightData pfd;

    pfd.angleOfAttack = mAuxiliary->Getalpha();
    pfd.sideSlip = mAuxiliary->Getbeta();
    pfd.climbRate = mPropagate->Gethdot();

    pfd.roll = mPropagate->GetEulerDeg(1);
    pfd.pitch = mPropagate->GetEulerDeg(2);
    pfd.heading = mPropagate->GetEulerDeg(3);

    pfd.airspeed = mAuxiliary->GetVcalibratedKTS();
    pfd.machNumber = mAuxiliary->GetMach();

    pfd.latitude = mPropagate->GetLatitudeDeg();
    pfd.longitude = mPropagate->GetLongitudeDeg();
    pfd.altitude = 0.3048 * mPropagate->GetGeodeticAltitude();

    pfd.rudderPos = qRadiansToDegrees(mCommander->GetDrPos());
    pfd.elevatorPos = qRadiansToDegrees(mCommander->GetDePos());
    pfd.leftAileronPos = qRadiansToDegrees(mCommander->GetDaLPos());
    pfd.rightAileronPos = qRadiansToDegrees(mCommander->GetDaRPos());

    JSBSim::FGQuaternion rotation = mPropagate->GetQuaternion();
    QQuaternion localToBody = QQuaternion(rotation(1), rotation(2), rotation(3), rotation(4));

    pfd.rotation = mConverter->ToOpenGL(pfd.latitude, pfd.longitude, localToBody);
    pfd.position = mConverter->ToOpenGL(pfd.latitude, pfd.longitude, pfd.altitude);

    emit PfdChanged(pfd);
}

void Aircraft::Stop()
{
    qInfo() << thread() << "Stopping Aircraft...";

    mTimer.stop();

    qInfo() << thread() << "Aircraft is stopped.";
}

bool Aircraft::GetHolding() const
{
    return mExecutor->Holding();
}