#include "GlobeCamera.h"

#include <QtMath>

Canavar::Engine::GlobeCamera::GlobeCamera()
{
    SetNodeName("Globe Camera");
    ApplyGeodeticTransform();
}

void Canavar::Engine::GlobeCamera::SetGeodeticPosition(double LatDeg, double LonDeg, double AltMeters)
{
    mLat = LatDeg;
    mLon = LonDeg;
    mAlt = AltMeters;
    ApplyGeodeticTransform();
}

Canavar::Engine::Wgs84::GeoPoint Canavar::Engine::GlobeCamera::GetGeodeticPosition() const
{
    return { mLat, mLon, mAlt };
}

void Canavar::Engine::GlobeCamera::ApplyGeodeticTransform()
{
    SetPosition(Wgs84::ToWorld(mLat, mLon, mAlt));

    // Build camera-to-world rotation aligned with the local ENU frame at (mLat, mLon).
    // Camera local axes map to: +X = East,  +Y = Up,  -Z = North (+Z = South).
    QMatrix3x3 Enu = Wgs84::EnuToWorld(mLat, mLon); // columns: East(0), North(1), Up(2)

    // Row-major values for QMatrix3x3 constructor.
    // mat(row, col): col 0 = East, col 1 = Up, col 2 = South = -North
    float Vals[9] = {
        Enu(0, 0), Enu(0, 2), -Enu(0, 1), // row 0
        Enu(1, 0), Enu(1, 2), -Enu(1, 1), // row 1
        Enu(2, 0), Enu(2, 2), -Enu(2, 1), // row 2
    };
    const QQuaternion BaseRot = QQuaternion::fromRotationMatrix(QMatrix3x3(Vals));

    const QQuaternion YawRot = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), mYaw);
    const QQuaternion PitchRot = QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), mPitch);

    // Qt multiplication: A*B applies B first. Order: pitch → yaw → base (ENU alignment).
    SetRotation(BaseRot * YawRot * PitchRot);
}

void Canavar::Engine::GlobeCamera::Update(float Ifps)
{
    if (mMouse.IsButtonPressed(Qt::MiddleButton))
    {
        const float AngSpeed = CalculateAngularSpeed(Ifps);
        mYaw -= AngSpeed * mMouse.GetCumulativeMovement(Qt::MiddleButton).x();
        mPitch -= AngSpeed * mMouse.GetCumulativeMovement(Qt::MiddleButton).y();
        mPitch = qBound(-89.0f, mPitch, 89.0f);
        mMouse.ResetCumulativeMovement(Qt::MiddleButton);
    }

    const float LinearSpeed = CalculateLinearSpeed(Ifps);
    QVector3D Move(0, 0, 0);

    for (const auto& [Key, IsPressed] : mPressedKeys.toStdMap())
    {
        if (IsPressed)
        {
            Move += GetRotation().rotatedVector(KEY_BINDINGS.value(Key, QVector3D(0, 0, 0)));
        }
    }

    if (!Move.isNull())
    {
        const QVector3D NewWorldPos = GetPosition() + Move * LinearSpeed;
        const Wgs84::GeoPoint Geo = Wgs84::ToGeodetic(NewWorldPos);
        mLat = Geo.Lat;
        mLon = Geo.Lon;
        mAlt = Geo.Alt;
    }

    ApplyGeodeticTransform();
}

void Canavar::Engine::GlobeCamera::Reset()
{
    mMouse.Reset();
    mPressedKeys.clear();
}

bool Canavar::Engine::GlobeCamera::OnKeyPressed(QKeyEvent* pEvent)
{
    mPressedKeys[(Qt::Key) pEvent->key()] = true;
    return false;
}

bool Canavar::Engine::GlobeCamera::OnKeyReleased(QKeyEvent* pEvent)
{
    mPressedKeys[(Qt::Key) pEvent->key()] = false;
    return false;
}

bool Canavar::Engine::GlobeCamera::OnMousePressed(QMouseEvent* pEvent)
{
    if (pEvent->button() == Qt::MiddleButton)
    {
        mMouse.SetButtonPressed(Qt::MiddleButton, true);
        mMouse.SetLastPressPosition(Qt::MiddleButton, pEvent->position());
        return true;
    }
    return false;
}

bool Canavar::Engine::GlobeCamera::OnMouseReleased(QMouseEvent* pEvent)
{
    if (pEvent->button() == Qt::MiddleButton)
    {
        mMouse.SetButtonPressed(Qt::MiddleButton, false);
        return true;
    }
    return false;
}

bool Canavar::Engine::GlobeCamera::OnMouseMoved(QMouseEvent* pEvent)
{
    if (mMouse.IsButtonPressed(Qt::MiddleButton))
    {
        const auto Movement = pEvent->position() - mMouse.GetLastPressPosition(Qt::MiddleButton);
        mMouse.AddCumulativeMovement(Qt::MiddleButton, Movement);
        mMouse.SetLastPressPosition(Qt::MiddleButton, pEvent->position());
        return true;
    }
    return false;
}

bool Canavar::Engine::GlobeCamera::OnLeaveEvent(QEvent*)
{
    mMouse.Reset();
    mPressedKeys.clear();
    return true;
}

bool Canavar::Engine::GlobeCamera::IsKeyPressed(Qt::Key Key) const
{
    return mPressedKeys.value(Key, false);
}

float Canavar::Engine::GlobeCamera::CalculateLinearSpeed(float Ifps) const
{
    if (IsKeyPressed(Qt::Key_Shift))
        return mLinearSpeed * 100.0f * Ifps;
    if (IsKeyPressed(Qt::Key_Space))
        return mLinearSpeed * 10.0f * Ifps;
    if (IsKeyPressed(Qt::Key_Control))
        return mLinearSpeed * 0.1f * Ifps;
    return mLinearSpeed * Ifps;
}

float Canavar::Engine::GlobeCamera::CalculateAngularSpeed(float Ifps) const
{
    return mAngularSpeed * Ifps;
}

const QMap<Qt::Key, QVector3D> Canavar::Engine::GlobeCamera::KEY_BINDINGS = {
    { Qt::Key_W, QVector3D(0, 0, -1) }, //
    { Qt::Key_S, QVector3D(0, 0, 1) },  //
    { Qt::Key_A, QVector3D(-1, 0, 0) }, //
    { Qt::Key_D, QVector3D(1, 0, 0) },  //
    { Qt::Key_E, QVector3D(0, 1, 0) },  //
    { Qt::Key_Q, QVector3D(0, -1, 0) },
};
