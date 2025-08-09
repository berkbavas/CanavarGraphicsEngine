#include "AircraftController.h"

#include <Canavar/Engine/Util/Logger.h>

Canavar::Simulator::AircraftController::AircraftController(Aircraft* aircraft, QObject* parent)
    : QObject(parent)
    , mAircraft(aircraft)
    , mAileron(0)
    , mElevator(0)
    , mRudder(0)
    , mThrottle(0)
    , mAutoPilotEnabled(false)
{
    connect(&mTimer, &QTimer::timeout, this, &Canavar::Simulator::AircraftController::Tick);
    connect(mAircraft, &Aircraft::PfdChanged, this, [=](Aircraft::PrimaryFlightData pfd) { mPfd = pfd; }, Qt::QueuedConnection);
    connect(this, &Canavar::Simulator::AircraftController::Command, mAircraft, &Aircraft::ProcessCommand, Qt::QueuedConnection);
}

void Canavar::Simulator::AircraftController::KeyPressed(QKeyEvent* event)
{
    mPressedKeys.insert((Qt::Key) event->key());
}

void Canavar::Simulator::AircraftController::KeyReleased(QKeyEvent* event)
{
    mPressedKeys.remove((Qt::Key) event->key());
}

const Canavar::Simulator::Aircraft::PrimaryFlightData& Canavar::Simulator::AircraftController::GetPrimaryFlightData() const
{
    return mPfd;
}

bool Canavar::Simulator::AircraftController::Initialize()
{
    emit Command(Aircraft::Command::Hold);

    mTimer.start(5);

    return true;
}

void Canavar::Simulator::AircraftController::Tick()
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

    if (mAutoPilotEnabled && mPressedKeys.isEmpty())
    {
        mElevator = GetCmd(Aircraft::Command::Elevator, 4.0).toDouble();
        mAileron = GetCmd(Aircraft::Command::Aileron, 0.0).toDouble();
    }

    mElevator = qBound(-1.0f, mElevator, 1.0f);
    mAileron = qBound(-1.0f, mAileron, 1.0f);
    mRudder = qBound(-1.0f, mRudder, 1.0f);
    mThrottle = qBound(0.0f, mThrottle, 1.0f);

    emit Command(Aircraft::Command::Elevator, mElevator);
    emit Command(Aircraft::Command::Aileron, mAileron);
    emit Command(Aircraft::Command::Rudder, mRudder);
    emit Command(Aircraft::Command::Throttle, mThrottle);

    mAircraft->Tick();
}

QVariant Canavar::Simulator::AircraftController::GetCmd(Aircraft::Command command, QVariant value)
{
    switch (command)
    {
    case Aircraft::Command::Aileron:
    {
        double cmd = (value.toDouble() - mPfd.roll) / 30.0;
        return qMax(-1.0, qMin(cmd, 1.0));
    }
    case Aircraft::Command::Elevator:
    {
        double cmd = (mPfd.pitch - value.toDouble()) / 20.0;
        return qMax(-1.0, qMin(cmd, 1.0));
    }
    default:
        break;
    }

    return QVariant();
}

void Canavar::Simulator::AircraftController::DrawGui()
{
    ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
    ImGui::Begin("Simulator");

    ImGui::BeginDisabled();
    ImGui::SliderFloat("Elevator", &mElevator, -1.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Aileron", &mAileron, -1.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Rudder", &mRudder, -1.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Throttle", &mThrottle, 0.0f, 1.0f, "%.3f");
    ImGui::EndDisabled();

    if (ImGui::SliderFloat("Smooth Position Multiplier", &mSmoothPositionCoefficent, 1.1f, 20.0f))
    {
        emit Command(Aircraft::Command::ChangeSmoothPositionCoefficient, mSmoothPositionCoefficent);
    }

    if (ImGui::Button("Init Running"))
    {
        emit Command(Aircraft::Command::InitRunning);
    }

    if (ImGui::Button("Hold"))
    {
        emit Command(Aircraft::Command::Hold);
    }

    if (ImGui::Button("Resume"))
    {
        emit Command(Aircraft::Command::Resume);
    }

    ImGui::Checkbox("Auto Pilot", &mAutoPilotEnabled);

    ImGui::Text("Airspeed:    %.2f knots", mPfd.airspeed);
    ImGui::Text("Latitude:    %.6f °", mPfd.latitude);
    ImGui::Text("Longitude:   %.6f °", mPfd.longitude);
    ImGui::Text("Altitude:    %.2f meters", mPfd.altitude);
    ImGui::Text("Roll:        %.1f °", mPfd.roll);
    ImGui::Text("Pitch:       %.1f °", mPfd.pitch);
    ImGui::Text("Heading:     %.1f °", mPfd.heading);
    ImGui::Text("Delta Pos:   %.2f meters", (mPfd.position - mPfd.actualPosition).length());
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}