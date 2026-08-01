#include "GlobeCamera.h"

#include <algorithm>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Canavar::Globe
{
    GlobeCamera::GlobeCamera()
    {
        RebuildProjection();
        RebuildEcef();
    }

    void GlobeCamera::SetGeoPosition(double LatDeg, double LonDeg, double AltMetres)
    {
        mLatDeg = std::clamp(LatDeg, -89.9, 89.9);
        mLonDeg = LonDeg;
        mAltM   = std::max(AltMetres, 10.0);
        RebuildEcef();
    }

    void GlobeCamera::SetHeading(double HeadingDeg) { mHeadingDeg = HeadingDeg; }
    void GlobeCamera::SetPitch(double PitchDeg)     { mPitchDeg = std::clamp(PitchDeg, -89.9, 89.9); }

    void GlobeCamera::AdjustAltitude(double DeltaM)
    {
        mAltM = std::max(mAltM + DeltaM, 10.0);
        RebuildEcef();
    }

    void GlobeCamera::RotateHeading(double DeltaDeg) { mHeadingDeg += DeltaDeg; }
    void GlobeCamera::RotatePitch(double DeltaDeg)
    {
        mPitchDeg = std::clamp(mPitchDeg + DeltaDeg, -89.9, 89.9);
    }

    void GlobeCamera::MoveForward(double DistMetres)
    {
        // Move in the horizontal plane along the heading direction.
        const double H    = mHeadingDeg * M_PI / 180.0;
        const double DLon = std::sin(H) * DistMetres / (Wgs84A * std::cos(mLatDeg * M_PI / 180.0)) * (180.0 / M_PI);
        const double DLat = std::cos(H) * DistMetres /  Wgs84A * (180.0 / M_PI);
        mLatDeg = std::clamp(mLatDeg + DLat, -89.9, 89.9);
        mLonDeg += DLon;
        RebuildEcef();
    }

    void GlobeCamera::MoveRight(double DistMetres)
    {
        const double H    = (mHeadingDeg + 90.0) * M_PI / 180.0;
        const double DLon = std::sin(H) * DistMetres / (Wgs84A * std::cos(mLatDeg * M_PI / 180.0)) * (180.0 / M_PI);
        const double DLat = std::cos(H) * DistMetres /  Wgs84A * (180.0 / M_PI);
        mLatDeg = std::clamp(mLatDeg + DLat, -89.9, 89.9);
        mLonDeg += DLon;
        RebuildEcef();
    }

    void GlobeCamera::SetAspectRatio(float Aspect) { mAspect = Aspect; RebuildProjection(); }
    void GlobeCamera::SetFovDeg(float FovDeg)       { mFovDeg = FovDeg; RebuildProjection(); }
    void GlobeCamera::SetNearFar(float Near, float Far)
    {
        mNear = Near; mFar = Far; RebuildProjection();
    }

    void GlobeCamera::RebuildProjection()
    {
        mProjection.setToIdentity();
        mProjection.perspective(mFovDeg, mAspect, mNear, mFar);
    }

    void GlobeCamera::RebuildEcef()
    {
        mEcef = GeodeticToECEF(mLatDeg, mLonDeg, mAltM);
    }

    QMatrix4x4 GlobeCamera::ViewMatrix() const
    {
        // Camera is at origin in ECEF-relative space.
        // Build look-direction from heading + pitch in the local ENU frame.
        const ENUBasis Basis = ComputeENUBasis(mLatDeg, mLonDeg);

        const double H  = mHeadingDeg * M_PI / 180.0;
        const double P  = mPitchDeg   * M_PI / 180.0;

        // Look direction in ENU: (east, north, up) components
        const double Lx = std::sin(H) * std::cos(P);
        const double Ly = std::cos(H) * std::cos(P);
        const double Lz = std::sin(P);

        // Up direction in ENU
        const double Ux = -std::sin(H) * std::sin(P);
        const double Uy = -std::cos(H) * std::sin(P);
        const double Uz =  std::cos(P);

        // Convert ENU → ECEF
        auto ENUtoECEF = [&](double Ex, double Ey, double Ez) -> QVector3D
        {
            return {
                static_cast<float>(Basis.East[0]*Ex + Basis.North[0]*Ey + Basis.Up[0]*Ez),
                static_cast<float>(Basis.East[1]*Ex + Basis.North[1]*Ey + Basis.Up[1]*Ez),
                static_cast<float>(Basis.East[2]*Ex + Basis.North[2]*Ey + Basis.Up[2]*Ez)
            };
        };

        const QVector3D LookDir = ENUtoECEF(Lx, Ly, Lz).normalized();
        const QVector3D UpDir   = ENUtoECEF(Ux, Uy, Uz).normalized();

        QMatrix4x4 View;
        View.setToIdentity();
        View.lookAt(QVector3D(0, 0, 0), LookDir, UpDir);
        return View;
    }

} // namespace Canavar::Globe
