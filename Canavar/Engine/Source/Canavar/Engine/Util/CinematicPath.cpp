#include "CinematicPath.h"

#include "Canavar/Engine/Camera/PerspectiveCamera.h"

#include <QtMath>

Canavar::Engine::CinematicPath::CinematicPath(QObject *pParent)
    : QObject(pParent)
{}

// ─────────────────────────────────────────────────────────────────────────────
// Waypoint management
// ─────────────────────────────────────────────────────────────────────────────

void Canavar::Engine::CinematicPath::AddWaypoint(const Waypoint &Wp)
{
    mWaypoints.push_back(Wp);
}

void Canavar::Engine::CinematicPath::InsertWaypoint(int Index, const Waypoint &Wp)
{
    if (Index < 0 || Index > mWaypoints.size())
        return;
    mWaypoints.insert(Index, Wp);
}

void Canavar::Engine::CinematicPath::RemoveWaypoint(int Index)
{
    if (Index < 0 || Index >= mWaypoints.size())
        return;
    mWaypoints.remove(Index);
}

void Canavar::Engine::CinematicPath::ClearWaypoints()
{
    mWaypoints.clear();
}

// ─────────────────────────────────────────────────────────────────────────────
// Playback control
// ─────────────────────────────────────────────────────────────────────────────

void Canavar::Engine::CinematicPath::Play()
{
    if (mWaypoints.size() < 2)
        return;
    mPlaying = true;
    mElapsedTime = 0.0f;
}

void Canavar::Engine::CinematicPath::Pause()
{
    mPlaying = false;
}

void Canavar::Engine::CinematicPath::Stop()
{
    mPlaying = false;
    mElapsedTime = 0.0f;
}

float Canavar::Engine::CinematicPath::GetTotalDuration() const
{
    float Total = 0.0f;
    // Duration of waypoint i = travel time from i to i+1; last waypoint's duration is unused.
    for (int i = 0; i < mWaypoints.size() - 1; ++i)
        Total += mWaypoints[i].Duration;
    return Total;
}

float Canavar::Engine::CinematicPath::GetProgress() const
{
    const float Total = GetTotalDuration();
    return (Total > 0.0f) ? qBound(0.0f, mElapsedTime / Total, 1.0f) : 0.0f;
}

// ─────────────────────────────────────────────────────────────────────────────
// Per-frame update
// ─────────────────────────────────────────────────────────────────────────────

void Canavar::Engine::CinematicPath::Update(float Ifps)
{
    if (!mPlaying || mWaypoints.size() < 2 || !mCamera)
        return;

    mElapsedTime += Ifps * mSpeedMultiplier;

    const float Total = GetTotalDuration();

    if (mElapsedTime >= Total)
    {
        if (mLooping)
        {
            mElapsedTime = fmodf(mElapsedTime, Total);
        }
        else
        {
            mElapsedTime = Total;
            mPlaying = false;

            // Snap to the last waypoint
            const Waypoint &Last = mWaypoints.last();
            mCamera->SetPosition(Last.Position);
            mCamera->SetRotation(Last.Rotation);

            emit Finished();
            return;
        }
    }

    QVector3D Position;
    QQuaternion Rotation;
    EvaluateAtTime(mElapsedTime, Position, Rotation);

    mCamera->SetPosition(Position);
    mCamera->SetRotation(Rotation);
}

// ─────────────────────────────────────────────────────────────────────────────
// Spline evaluation
// ─────────────────────────────────────────────────────────────────────────────

// Catmull-Rom: interpolates between P1 and P2 with ghost points P0 and P3.
QVector3D Canavar::Engine::CinematicPath::CatmullRom(const QVector3D &P0, const QVector3D &P1, const QVector3D &P2, const QVector3D &P3, float T)
{
    const float T2 = T * T;
    const float T3 = T2 * T;

    return 0.5f * ((2.0f * P1) + (-P0 + P2) * T + (2.0f * P0 - 5.0f * P1 + 4.0f * P2 - P3) * T2 + (-P0 + 3.0f * P1 - 3.0f * P2 + P3) * T3);
}

void Canavar::Engine::CinematicPath::EvaluateAtTime(float Time, QVector3D &OutPosition, QQuaternion &OutRotation) const
{
    const int N = mWaypoints.size();

    // Find which segment we are in and the local t ∈ [0,1] within it.
    float Accumulated = 0.0f;
    int Segment = 0;
    float LocalT = 0.0f;

    for (int i = 0; i < N - 1; ++i)
    {
        const float SegDuration = mWaypoints[i].Duration;
        if (Time <= Accumulated + SegDuration || i == N - 2)
        {
            Segment = i;
            LocalT = (SegDuration > 0.0f) ? qBound(0.0f, (Time - Accumulated) / SegDuration, 1.0f) : 0.0f;
            break;
        }
        Accumulated += SegDuration;
    }

    // Ghost/clamped indices for Catmull-Rom
    const int I0 = qMax(Segment - 1, 0);
    const int I1 = Segment;
    const int I2 = qMin(Segment + 1, N - 1);
    const int I3 = qMin(Segment + 2, N - 1);

    OutPosition = CatmullRom(mWaypoints[I0].Position, mWaypoints[I1].Position, mWaypoints[I2].Position, mWaypoints[I3].Position, LocalT);

    OutRotation = QQuaternion::slerp(mWaypoints[I1].Rotation, mWaypoints[I2].Rotation, LocalT);
}
