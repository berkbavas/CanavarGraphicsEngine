#pragma once

// ─────────────────────────────────────────────────────────────────────────────
//  GlobeCamera
//  Camera positioned on the WGS-84 ellipsoid surface.
//  Position is stored in double precision (lat/lon/altitude) to avoid
//  floating-point artefacts when close to the terrain.
// ─────────────────────────────────────────────────────────────────────────────

#include "TerrainCommon.h"

#include <QMatrix4x4>
#include <QVector3D>

namespace Canavar::Globe
{
    class GlobeCamera
    {
      public:
        GlobeCamera();

        // ── position ──────────────────────────────────────────────────────────
        void SetGeoPosition(double LatDeg, double LonDeg, double AltMetres);
        double Latitude()  const { return mLatDeg; }
        double Longitude() const { return mLonDeg; }
        double Altitude()  const { return mAltM; }

        // ── orientation ───────────────────────────────────────────────────────
        void SetHeading(double HeadingDeg);  // 0=North, 90=East
        void SetPitch(double PitchDeg);      // 0=level, -90=straight up
        double Heading() const { return mHeadingDeg; }
        double Pitch()   const { return mPitchDeg; }

        // ── movement helpers (for keyboard/mouse input) ────────────────────────
        void MoveForward(double DistMetres);  // along heading direction
        void MoveRight(double DistMetres);    // perpendicular to heading
        void AdjustAltitude(double DeltaM);
        void RotateHeading(double DeltaDeg);
        void RotatePitch(double DeltaDeg);

        // ── matrices ──────────────────────────────────────────────────────────
        void SetAspectRatio(float Aspect);
        void SetFovDeg(float FovDeg);
        void SetNearFar(float Near, float Far);

        QMatrix4x4 ProjectionMatrix() const { return mProjection; }

        // View matrix expressed in the camera's ECEF-relative space
        // (camera is at the origin of this space).
        QMatrix4x4 ViewMatrix() const;

        // Camera ECEF origin used as the "subtraction point" for vertex positions.
        std::array<double, 3> EcefOrigin() const { return mEcef; }

      private:
        void RebuildProjection();
        void RebuildEcef();

        double mLatDeg{ 41.0 };
        double mLonDeg{ 29.0 };
        double mAltM{ 5000.0 };
        double mHeadingDeg{ 0.0 };
        double mPitchDeg{ -45.0 };

        std::array<double, 3> mEcef{};

        float mAspect{ 16.0f / 9.0f };
        float mFovDeg{ 60.0f };
        float mNear{ 10.0f };
        float mFar{ 20'000'000.0f };

        QMatrix4x4 mProjection;
    };

} // namespace Canavar::Globe
