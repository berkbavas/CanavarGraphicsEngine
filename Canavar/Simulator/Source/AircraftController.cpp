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
    , mTimeElapsed(0.0f)
{
    connect(this, &Canavar::Simulator::AircraftController::Command, mAircraft, &Aircraft::ProcessCommand, Qt::QueuedConnection);
    connect(&mTimer, &QTimer::timeout, this, &Canavar::Simulator::AircraftController::Tick);
    connect(mAircraft, &Aircraft::PfdChanged, this, [=](Aircraft::PrimaryFlightData pfd) { mPfd = pfd; }, Qt::QueuedConnection);
}

void Canavar::Simulator::AircraftController::KeyPressed(QKeyEvent* event)
{
    mPressedKeys.insert((Qt::Key) event->key());
}

void Canavar::Simulator::AircraftController::KeyReleased(QKeyEvent* event)
{
    mPressedKeys.remove((Qt::Key) event->key());
}

bool Canavar::Simulator::AircraftController::Initialize()
{
    emit Command(Aircraft::Command::Hold);

    mTimer.start(10);

    return true;
}

void Canavar::Simulator::AircraftController::Tick()
{
    if (mPressedKeys.contains(Qt::Key_Up))
        mElevator += 0.025;
    else if (mPressedKeys.contains(Qt::Key_Down))
        mElevator -= 0.025;
    else if (mElevator < -0.025)
        mElevator += 0.025;
    else if (mElevator > 0.025)
        mElevator -= 0.025;
    else
        mElevator = 0.0;

    if (mPressedKeys.contains(Qt::Key_Left))
        mAileron -= 0.025;
    else if (mPressedKeys.contains(Qt::Key_Right))
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

void Canavar::Simulator::AircraftController::Update(float ifps)
{
    mTimeElapsed += ifps;

    if (mRootNode == nullptr)
    {
        CGE_EXIT_FAILURE("mRootNode is nullptr. Exiting...");
    }

    // if (mJetNode == nullptr)
    // {
    //     CGE_EXIT_FAILURE("mJetNode is nullptr. Exiting...");
    // }

    mRootNode->SetWorldRotation(mPfd.rotation);
    mRootNode->SetWorldPosition(mPfd.position);

    // Rudder
    // if (mJetNode)
    // {
    //     QVector3D p0 = QVector3D(0.0f, 3.0193f, 10.3473f);
    //     QVector3D p1 = QVector3D(0.0f, 7.742f, 13.4306f);
    //     QVector3D axis = (p0 - p1).normalized();
    //     QMatrix4x4 t0, t1, t2;

    //     t0.translate(-p0);
    //     t1.rotate(QQuaternion::fromAxisAndAngle(axis, mPfd.rudderPos));
    //     t2.translate(p0);
    //     mJetNode->SetMeshTransformation("Object_18", t2 * t1 * t0);
    // }

    // // Left elevator
    // if (mJetNode)
    // {
    //     QVector3D p0 = QVector3D(-2.6f, 0.4204f, 8.4395f);
    //     QVector3D p1 = QVector3D(-6.8575f, -0.4848f, 11.7923f);
    //     QVector3D axis = (p0 - p1).normalized();
    //     QMatrix4x4 t0, t1, t2;

    //     t0.translate(-p0);
    //     t1.rotate(QQuaternion::fromAxisAndAngle(axis, -mPfd.elevatorPos));
    //     t2.translate(p0);
    //     mJetNode->SetMeshTransformation("Object_16", t2 * t1 * t0);
    // }

    // // Right elevator
    // if (mJetNode)
    // {
    //     QVector3D p0 = QVector3D(2.6f, 0.4204f, 8.4395f);
    //     QVector3D p1 = QVector3D(6.8575f, -0.4848f, 11.7923f);
    //     QVector3D axis = (p0 - p1).normalized();
    //     QMatrix4x4 t0, t1, t2;

    //     t0.translate(-p0);
    //     t1.rotate(QQuaternion::fromAxisAndAngle(axis, mPfd.elevatorPos));
    //     t2.translate(p0);
    //     mJetNode->SetMeshTransformation("Object_7", t2 * t1 * t0);
    // }

    // // Left aileron
    // if (mJetNode)
    // {
    //     QVector3D p0 = QVector3D(-2.6074f, 0.3266f, 3.4115f);
    //     QVector3D p1 = QVector3D(-8.7629f, -0.2083f, 4.333f);
    //     QVector3D axis = (p0 - p1).normalized();
    //     QMatrix4x4 t0, t1, t2;

    //     t0.translate(-p0);
    //     t1.rotate(QQuaternion::fromAxisAndAngle(axis, -mPfd.leftAileronPos));
    //     t2.translate(p0);
    //     mJetNode->SetMeshTransformation("Object_9", t2 * t1 * t0);
    // }

    // // Right aileron
    // if (mJetNode)
    // {
    //     QVector3D p0 = QVector3D(2.6072f, 0.3266f, 3.4115f);
    //     QVector3D p1 = QVector3D(8.7623f, 0.1772f, 4.3218f);
    //     QVector3D axis = (p0 - p1).normalized();
    //     QMatrix4x4 t0, t1, t2;

    //     t0.translate(-p0);
    //     t1.rotate(QQuaternion::fromAxisAndAngle(axis, mPfd.rightAileronPos));
    //     t2.translate(p0);
    //     mJetNode->SetMeshTransformation("Object_8", t2 * t1 * t0);
    // }
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

    if (ImGui::Button("Init Running"))
        emit Command(Aircraft::Command::InitRunning);

    if (ImGui::Button("Hold"))
        emit Command(Aircraft::Command::Hold);

    if (ImGui::Button("Resume"))
        emit Command(Aircraft::Command::Resume);

    ImGui::Checkbox("Auto Pilot", &mAutoPilotEnabled);

    ImGui::Text("Airspeed:    %.2f knots", mPfd.airspeed);
    ImGui::Text("Latitude:    %.6f °", mPfd.latitude);
    ImGui::Text("Longitude:   %.6f °", mPfd.longitude);
    ImGui::Text("Altitude:    %.2f meters", mPfd.altitude);
    ImGui::Text("Roll:        %.1f °", mPfd.roll);
    ImGui::Text("Pitch:       %.1f °", mPfd.pitch);
    ImGui::Text("Heading:     %.1f °", mPfd.heading);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}

void Canavar::Simulator::AircraftController::SetRootNode(Canavar::Engine::ObjectPtr pRootNode)
{
    mRootNode = pRootNode;
}

void Canavar::Simulator::AircraftController::SetJetNode(Canavar::Engine::ModelPtr pJetNode)
{
    mJetNode = pJetNode;
}