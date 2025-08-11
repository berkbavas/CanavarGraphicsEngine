#include "Aircraft.h"

#include "Constants.h"

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

Canavar::Simulator::Aircraft::~Aircraft()
{
    if (mExecutor)
    {
        delete mExecutor;
        mExecutor = nullptr;
    }

    if (mConverter)
    {
        delete mConverter;
        mConverter = nullptr;
    }
}

bool Canavar::Simulator::Aircraft::Initialize()
{
    QString CurrentPath = QDir::currentPath();

    QDir::setCurrent(RESOURCE_PATH);

    mExecutor = new JSBSim::FGFDMExec;
    mExecutor->SetDebugLevel(0);
    mExecutor->DisableOutput();

    LOG_INFO("Aircraft::Initialize: Aircraft is being initializing...");

    if (!mExecutor->LoadModel(JSBSIM_MODEL_PATH))
    {
        LOG_FATAL("Aircraft::Initialize: Could not load JSBSim model.");
        return false;
    }

    if (!mExecutor->GetIC()->Load(SGPath(JSBSIM_INITIAL_CONDITIONS_PATH)))
    {
        LOG_FATAL("Aircraft::Initialize: Could not load JSBSim initial conditions.");
        return false;
    }

    if (!mExecutor->RunIC())
    {
        LOG_FATAL("Aircraft::Initialize: Could not run JSBSim initial conditions.");
        return false;
    }

    mCommander = mExecutor->GetFCS();
    mPropagate = mExecutor->GetPropagate();
    mPropulsion = mExecutor->GetPropulsion();
    mAuxiliary = mExecutor->GetAuxiliary();

    mRefLatitude = mExecutor->GetIC()->GetLatitudeDegIC();
    mRefLongitude = mExecutor->GetIC()->GetLongitudeDegIC();
    mRefAltitude = mExecutor->GetIC()->GetAltitudeASLFtIC() * FEET_TO_METER; // Convert feet to meters;

    mConverter = new Converter(mRefLatitude, mRefLongitude, 0.0); // Reference altitude is set to 0.0

    // Restore the current path
    QDir::setCurrent(CurrentPath);

    LOG_INFO("Aircraft::Initialize: Aircraft is initialized.");

    ProcessCommand(Command::Hold);

    return true;
}

void Canavar::Simulator::Aircraft::Tick(float ifps)
{
    ProcessAutoPilotIfEnabled();
    ProcessInputs();

    // Clamp control surfaces
    mElevator = qBound(-1.0f, mElevator, 1.0f);
    mAileron = qBound(-1.0f, mAileron, 1.0f);
    mRudder = qBound(-1.0f, mRudder, 1.0f);
    mThrottle = qBound(0.0f, mThrottle, 1.0f);

    ProcessCommand(Command::Elevator, mElevator);
    ProcessCommand(Command::Aileron, mAileron);
    ProcessCommand(Command::Rudder, mRudder);
    ProcessCommand(Command::Throttle, mThrottle);

    Run(ifps);
}

void Canavar::Simulator::Aircraft::DrawGui()
{
    ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
    ImGui::Begin("Simulator");

    ImGui::BeginDisabled();
    ImGui::SliderFloat("Elevator", &mElevator, -1.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Aileron", &mAileron, -1.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Rudder", &mRudder, -1.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Throttle", &mThrottle, 0.0f, 1.0f, "%.3f");
    ImGui::EndDisabled();

    if (ImGui::Button("Init Running"))
    {
        ProcessCommand(Command::InitRunning);
    }

    if (ImGui::Button("Hold"))
    {
        ProcessCommand(Command::Hold);
    }

    if (ImGui::Button("Resume"))
    {
        ProcessCommand(Command::Resume);
    }

    ImGui::Checkbox("Auto Pilot", &mAutoPilotEnabled);

    ImGui::Text("Airspeed:    %.2f knots", mPfd.Airspeed);
    ImGui::Text("Latitude:    %.6f °", mPfd.Latitude);
    ImGui::Text("Longitude:   %.6f °", mPfd.Longitude);
    ImGui::Text("Altitude:    %.2f meters", mPfd.Altitude);
    ImGui::Text("Roll:        %.1f °", mPfd.Roll);
    ImGui::Text("Pitch:       %.1f °", mPfd.Pitch);
    ImGui::Text("Heading:     %.1f °", mPfd.Heading);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}

void Canavar::Simulator::Aircraft::OnKeyPressed(QKeyEvent* pEvent)
{
    mPressedKeys.insert((Qt::Key) pEvent->key());
}

void Canavar::Simulator::Aircraft::OnKeyReleased(QKeyEvent* pEvent)
{
    mPressedKeys.remove((Qt::Key) pEvent->key());
}

void Canavar::Simulator::Aircraft::ProcessCommand(Command command, QVariant variant)
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

bool Canavar::Simulator::Aircraft::IsHolding() const
{
    return mExecutor->Holding();
}

const Canavar::Simulator::PrimaryFlightData& Canavar::Simulator::Aircraft::GetPfd() const
{
    return mPfd;
}

void Canavar::Simulator::Aircraft::Run(float ifps)
{
    mPropagate->SetTerrainElevation(TERRAIN_ELEVATION);

    mExecutor->Setdt(ifps);
    mExecutor->Run();

    mPfd.AngleOfAttack = mAuxiliary->Getalpha();
    mPfd.SideSlip = mAuxiliary->Getbeta();
    mPfd.ClimbRate = mPropagate->Gethdot();

    mPfd.Roll = mPropagate->GetEulerDeg(1);
    mPfd.Pitch = mPropagate->GetEulerDeg(2);
    mPfd.Heading = mPropagate->GetEulerDeg(3);

    mPfd.Airspeed = mAuxiliary->GetVcalibratedKTS();
    mPfd.MachNumber = mAuxiliary->GetMach();

    mPfd.Pressure = mAuxiliary->GetTotalPressure();

    mPfd.Latitude = mPropagate->GetLatitudeDeg();
    mPfd.Longitude = mPropagate->GetLongitudeDeg();
    mPfd.Altitude = FEET_TO_METER * mPropagate->GetGeodeticAltitude();

    mPfd.RudderPos = qRadiansToDegrees(mCommander->GetDrPos());
    mPfd.ElevatorPos = qRadiansToDegrees(mCommander->GetDePos());
    mPfd.LeftAileronPos = qRadiansToDegrees(mCommander->GetDaLPos());
    mPfd.RightAileronPos = qRadiansToDegrees(mCommander->GetDaRPos());

    // Rotation
    JSBSim::FGQuaternion Rotation = mPropagate->GetQuaternion();
    QQuaternion QtRotation = QQuaternion(Rotation(1), Rotation(2), Rotation(3), Rotation(4));
    mPfd.Rotation = mConverter->ConvertRotation(mPfd.Latitude, mPfd.Longitude, QtRotation);

    // Position
    mPfd.Position = mConverter->ConvertPositionToCartesian(mPfd.Latitude, mPfd.Longitude, mPfd.Altitude);
}

void Canavar::Simulator::Aircraft::ProcessInputs()
{
    if (mPressedKeys.contains(Qt::Key_Up) || mPressedKeys.contains(Qt::Key_W))
        mElevator += 0.025;
    else if (mPressedKeys.contains(Qt::Key_Down) || mPressedKeys.contains(Qt::Key_S))
        mElevator -= 0.025;
    else if (mElevator < -0.025)
        mElevator += 0.025;
    else if (mElevator > 0.025)
        mElevator -= 0.025;
    else
        mElevator = 0.0;

    if (mPressedKeys.contains(Qt::Key_Left) || mPressedKeys.contains(Qt::Key_A))
        mAileron -= 0.025;
    else if (mPressedKeys.contains(Qt::Key_Right) || mPressedKeys.contains(Qt::Key_D))
        mAileron += 0.025;
    else if (mAileron < -0.025)
        mAileron += 0.025;
    else if (mAileron > 0.025)
        mAileron -= 0.025;
    else
        mAileron = 0.0;

    if (mPressedKeys.contains(Qt::Key_Z))
        mRudder += 0.025;
    else if (mPressedKeys.contains(Qt::Key_C))
        mRudder -= 0.025;
    else if (mRudder < -0.025)
        mRudder += 0.025;
    else if (mRudder > 0.025)
        mRudder -= 0.025;
    else
        mRudder = 0.0;

    if (mPressedKeys.contains(Qt::Key_Plus))
        mThrottle += 0.01;

    if (mPressedKeys.contains(Qt::Key_Minus))
        mThrottle -= 0.01;
}

void Canavar::Simulator::Aircraft::ProcessAutoPilotIfEnabled()
{
    if (mAutoPilotEnabled && mPressedKeys.isEmpty())
    {
        mElevator = GetAutoPilotCommand(Command::Elevator, 1.0).toDouble();
        mAileron = GetAutoPilotCommand(Command::Aileron, 0.0).toDouble();
    }
}

QVariant Canavar::Simulator::Aircraft::GetAutoPilotCommand(Command command, QVariant value)
{
    switch (command)
    {
    case Command::Aileron:
    {
        double cmd = (value.toDouble() - mPfd.Roll) / 30.0;
        return qMax(-1.0, qMin(cmd, 1.0));
    }
    case Command::Elevator:
    {
        double cmd = (mPfd.Pitch - value.toDouble()) / 20.0;
        return qMax(-1.0, qMin(cmd, 1.0));
    }
    default:
        break;
    }

    return QVariant();
}
