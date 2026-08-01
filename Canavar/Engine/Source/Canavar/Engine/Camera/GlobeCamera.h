#pragma once

#include "Canavar/Engine/Camera/PerspectiveCamera.h"
#include "Canavar/Engine/Core/Mouse.h"
#include "Canavar/Engine/Util/Wgs84.h"

namespace Canavar::Engine
{
    // Camera that navigates on the WGS-84 ellipsoid.
    // Up axis always tracks the ellipsoid normal at the camera's geodetic position.
    // World +X = East, +Y = Up, +Z = South (looking North = camera -Z = identity).
    class GlobeCamera final : public PerspectiveCamera
    {
      public:
        GlobeCamera();

        const char* GetNodeTypeName() const override { return "GlobeCamera"; }

        void Update(float Ifps) override;
        void Reset() override;
        bool OnKeyPressed(QKeyEvent*) override;
        bool OnKeyReleased(QKeyEvent*) override;
        bool OnMousePressed(QMouseEvent*) override;
        bool OnMouseReleased(QMouseEvent*) override;
        bool OnMouseMoved(QMouseEvent*) override;
        bool OnLeaveEvent(QEvent*) override;

        void SetGeodeticPosition(double LatDeg, double LonDeg, double AltMeters);
        Wgs84::GeoPoint GetGeodeticPosition() const;

      private:
        void ApplyGeodeticTransform();
        float CalculateLinearSpeed(float Ifps) const;
        float CalculateAngularSpeed(float Ifps) const;
        bool IsKeyPressed(Qt::Key Key) const;

        double mLat{ 0.0 };
        double mLon{ 0.0 };
        double mAlt{ 1000.0 };

        float mYaw{ 0.0f };   // degrees, around local Up
        float mPitch{ 0.0f }; // degrees, around local East

        QMap<Qt::Key, bool> mPressedKeys;
        Mouse mMouse;

        DEFINE_MEMBER(float, AngularSpeed, 15.0f);
        DEFINE_MEMBER(float, LinearSpeed, 200.0f);

        static const QMap<Qt::Key, QVector3D> KEY_BINDINGS;
    };

    using GlobeCameraPtr = std::unique_ptr<GlobeCamera>;
}
