#include "Aircraft.h"

#include <FGFDMExec.h>
#include <FGJSBBase.h>
#include <initialization/FGInitialCondition.h>
#include <math/FGQuaternion.h>
#include <models/FGAuxiliary.h>
#include <models/FGFCS.h>
#include <models/FGPropagate.h>
#include <models/FGPropulsion.h>
#include <models/propulsion/FGThruster.h>

#include <QDebug>
#include <QDir>
#include <QtMath>

Canavar::Simulator::Aircraft::Aircraft(QObject* parent)
    : QObject(parent)
{}

bool Canavar::Simulator::Aircraft::Initialize()
{
    QString currentPath = QDir::currentPath();

    QDir::setCurrent("Resources/Data");

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

    // Restore the current path
    QDir::setCurrent(currentPath);

    qInfo() << "Aircraft is initialized.";

    return true;
}

void Canavar::Simulator::Aircraft::ProcessCommand(Canavar::Simulator::Aircraft::Command command, QVariant variant)
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

void Canavar::Simulator::Aircraft::Tick()
{
    mPropagate->SetTerrainElevation(0);

    mExecutor->Run();

    mPfd.angleOfAttack = mAuxiliary->Getalpha();
    mPfd.sideSlip = mAuxiliary->Getbeta();
    mPfd.climbRate = mPropagate->Gethdot();

    mPfd.roll = mPropagate->GetEulerDeg(1);
    mPfd.pitch = mPropagate->GetEulerDeg(2);
    mPfd.heading = mPropagate->GetEulerDeg(3);

    mPfd.airspeed = mAuxiliary->GetVcalibratedKTS();
    mPfd.machNumber = mAuxiliary->GetMach();

    mPfd.latitude = mPropagate->GetLatitudeDeg();
    mPfd.longitude = mPropagate->GetLongitudeDeg();
    mPfd.altitude = 0.3048 * mPropagate->GetGeodeticAltitude();

    mPfd.rudderPos = qRadiansToDegrees(mCommander->GetDrPos());
    mPfd.elevatorPos = qRadiansToDegrees(mCommander->GetDePos());
    mPfd.leftAileronPos = qRadiansToDegrees(mCommander->GetDaLPos());
    mPfd.rightAileronPos = qRadiansToDegrees(mCommander->GetDaRPos());

    JSBSim::FGQuaternion rotation = mPropagate->GetQuaternion();
    QQuaternion localToBody = QQuaternion(rotation(1), rotation(2), rotation(3), rotation(4));

    mPfd.rotation = mConverter->ConvertRotation(mPfd.latitude, mPfd.longitude, localToBody);
    mPfd.position = mConverter->ConvertPositionToCartesian(mPfd.latitude, mPfd.longitude, mPfd.altitude);

    emit PfdChanged(mPfd);
}

bool Canavar::Simulator::Aircraft::IsHolding() const
{
    return mExecutor->Holding();
}
