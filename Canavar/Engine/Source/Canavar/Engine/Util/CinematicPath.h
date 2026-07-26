#pragma once

#include <QMatrix4x4>
#include <QObject>
#include <QQuaternion>
#include <QVector3D>
#include <QVector>

namespace Canavar::Engine
{
    class PerspectiveCamera;

    // ─────────────────────────────────────────────────────────────────────────
    // CinematicPath
    //
    // Animates a camera along a Catmull-Rom spline through a sequence of
    // waypoints.  Each waypoint captures a world-space position and rotation.
    // The duration between consecutive waypoints is configurable per-segment.
    //
    // Usage:
    //   1. Add waypoints via AddWaypoint() or CaptureCurrentPose().
    //   2. Call Play() to start playback.
    //   3. Call Update(ifps) every frame (e.g. from Renderer::Updated signal).
    // ─────────────────────────────────────────────────────────────────────────
    class CinematicPath : public QObject
    {
        Q_OBJECT

      public:
        struct Waypoint
        {
            QVector3D Position{};
            QQuaternion Rotation{};
            float Duration{ 2.0f }; // seconds to travel FROM this waypoint TO the next
        };

        explicit CinematicPath(QObject *pParent = nullptr);

        // ── Waypoint management ───────────────────────────────────────────────
        void AddWaypoint(const Waypoint &Wp);
        void InsertWaypoint(int Index, const Waypoint &Wp);
        void RemoveWaypoint(int Index);
        void ClearWaypoints();
        const QVector<Waypoint> &GetWaypoints() const { return mWaypoints; }
        Waypoint &GetWaypoint(int Index) { return mWaypoints[Index]; }
        int GetWaypointCount() const { return mWaypoints.size(); }

        // ── Playback control ──────────────────────────────────────────────────
        void Play();
        void Pause();
        void Stop();

        bool IsPlaying() const { return mPlaying; }
        float GetProgress() const; // [0, 1]
        float GetElapsedTime() const { return mElapsedTime; }
        float GetTotalDuration() const;

        // ── Camera binding ────────────────────────────────────────────────────
        void SetCamera(PerspectiveCamera *pCamera) { mCamera = pCamera; }
        PerspectiveCamera *GetCamera() const { return mCamera; }

        bool IsLooping() const { return mLooping; }
        void SetLooping(bool Loop) { mLooping = Loop; }

        float GetSpeedMultiplier() const { return mSpeedMultiplier; }
        void SetSpeedMultiplier(float S) { mSpeedMultiplier = S; }

        // ── Per-frame update (call from Renderer::Updated slot) ───────────────
        void Update(float Ifps);

      signals:
        void Finished();

      private:
        // Catmull-Rom interpolation between p1 and p2, with ghost points p0 and p3.
        static QVector3D CatmullRom(const QVector3D &P0, const QVector3D &P1, const QVector3D &P2, const QVector3D &P3, float T);

        // Evaluate the spline at a given global time.
        void EvaluateAtTime(float T, QVector3D &OutPosition, QQuaternion &OutRotation) const;

        QVector<Waypoint> mWaypoints;
        PerspectiveCamera *mCamera{ nullptr };

        bool mPlaying{ false };
        bool mLooping{ false };
        float mElapsedTime{ 0.0f };
        float mSpeedMultiplier{ 1.0f };
    };
}
