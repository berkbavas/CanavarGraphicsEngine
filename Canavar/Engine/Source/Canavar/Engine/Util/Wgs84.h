#pragma once

#include <QMatrix3x3>
#include <QVector3D>

namespace Canavar::Engine
{
    // WGS-84 geodetic ↔ world-space conversions.
    //
    // World coordinate convention (local ENU at the world origin):
    //   +X = East,  +Y = Up (ellipsoid normal at origin),  +Z = South
    //
    // Call SetWorldOrigin() once at startup before using ToWorld / ToGeodetic.
    class Wgs84
    {
      public:
        struct GeoPoint
        {
            double Lat{ 0.0 }; // degrees
            double Lon{ 0.0 }; // degrees
            double Alt{ 0.0 }; // meters above ellipsoid
        };

        // Set the world-space origin (local ENU reference point). Call before any conversion.
        static void SetWorldOrigin(double LatDeg, double LonDeg, double AltMeters = 0.0);
        static GeoPoint GetWorldOrigin();

        // Geodetic ↔ engine world space
        static QVector3D ToWorld(double LatDeg, double LonDeg, double AltMeters);
        static GeoPoint ToGeodetic(const QVector3D& WorldPos);

        // Ellipsoid normal at (lat, lon) expressed in world coordinates (~(0,1,0) near origin)
        static QVector3D EllipsoidNormal(double LatDeg, double LonDeg);

        // ENU rotation at (lat, lon): columns are East, North, Up expressed in world space.
        // Use this to align camera/object orientation with the local geodetic frame.
        static QMatrix3x3 EnuToWorld(double LatDeg, double LonDeg);

        // WGS-84 semi-major axis (meters)
        static constexpr double SemiMajorAxis = 6378137.0;

      private:
        static constexpr double A = 6378137.0;
        static constexpr double B = 6356752.314245;
        static constexpr double E2 = 1.0 - (B * B) / (A * A); // first eccentricity squared

        static double N(double LatRad);

        static void GeodeticToEcef(double LatDeg, double LonDeg, double AltM, double& X, double& Y, double& Z);
        static GeoPoint EcefToGeodetic(double X, double Y, double Z);

        // ENU basis at (lat, lon) in ECEF space. Vectors are unit-length.
        static void ComputeEnuEcef(double LatDeg, double LonDeg, double East[3], double North[3], double Up[3]);

        // Dot product of two ECEF 3-vectors
        static double Dot(const double A[3], const double B[3]);

        static GeoPoint sOrigin;
        static double sOriginEcef[3];
        // World frame basis vectors in ECEF (unit vectors):
        //   sWorldBasis[0] = East  (world +X)
        //   sWorldBasis[1] = Up    (world +Y)
        //   sWorldBasis[2] = South (world +Z)
        static double sWorldBasis[3][3];
    };
}
