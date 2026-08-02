#include "Wgs84.h"

#include <cmath>

namespace Canavar::Engine
{
    // Static storage
    Wgs84::GeoPoint Wgs84::sOrigin{};
    double Wgs84::sOriginEcef[3]{};
    double Wgs84::sWorldBasis[3][3]{};

    // ─── Private helpers ──────────────────────────────────────────────────────────

    double Wgs84::N(double LatRad)
    {
        const double s = std::sin(LatRad);
        return A / std::sqrt(1.0 - E2 * s * s);
    }

    void Wgs84::GeodeticToEcef(double LatDeg, double LonDeg, double AltM, double& X, double& Y, double& Z)
    {
        const double lat = qDegreesToRadians(LatDeg);
        const double lon = qDegreesToRadians(LonDeg);
        const double n = N(lat);
        X = (n + AltM) * std::cos(lat) * std::cos(lon);
        Y = (n + AltM) * std::cos(lat) * std::sin(lon);
        Z = (n * (1.0 - E2) + AltM) * std::sin(lat);
    }

    Wgs84::GeoPoint Wgs84::EcefToGeodetic(double X, double Y, double Z)
    {
        const double p = std::sqrt(X * X + Y * Y);
        const double lon = std::atan2(Y, X);
        double lat = std::atan2(Z, p * (1.0 - E2)); // initial estimate

        for (int i = 0; i < 5; ++i)
        {
            const double sinLat = std::sin(lat);
            const double cosLat = std::cos(lat);
            const double n = N(lat);
            lat = std::atan2(Z + E2 * n * sinLat, p);
            (void) cosLat;
        }

        const double sinLat = std::sin(lat);
        const double cosLat = std::cos(lat);
        const double n = N(lat);
        const double alt = (std::abs(cosLat) > 1e-10) ? (p / cosLat - n) : (Z / sinLat - n * (1.0 - E2));
        return { qRadiansToDegrees(lat), qRadiansToDegrees(lon), alt };
    }

    void Wgs84::ComputeEnuEcef(double LatDeg, double LonDeg, double East[3], double North[3], double Up[3])
    {
        const double lat = qDegreesToRadians(LatDeg);
        const double lon = qDegreesToRadians(LonDeg);
        const double sinLat = std::sin(lat), cosLat = std::cos(lat);
        const double sinLon = std::sin(lon), cosLon = std::cos(lon);

        if (East)
        {
            East[0] = -sinLon;
            East[1] = cosLon;
            East[2] = 0.0;
        }
        if (North)
        {
            North[0] = -sinLat * cosLon;
            North[1] = -sinLat * sinLon;
            North[2] = cosLat;
        }
        if (Up)
        {
            Up[0] = cosLat * cosLon;
            Up[1] = cosLat * sinLon;
            Up[2] = sinLat;
        }
    }

    double Wgs84::Dot(const double A[3], const double B[3])
    {
        return A[0] * B[0] + A[1] * B[1] + A[2] * B[2];
    }

    // ─── Public API ───────────────────────────────────────────────────────────────

    void Wgs84::SetWorldOrigin(double LatDeg, double LonDeg, double AltMeters)
    {
        sOrigin = { LatDeg, LonDeg, AltMeters };
        GeodeticToEcef(LatDeg, LonDeg, AltMeters, sOriginEcef[0], sOriginEcef[1], sOriginEcef[2]);

        double East[3], North[3], Up[3];
        ComputeEnuEcef(LatDeg, LonDeg, East, North, Up);

        // World +X = East
        sWorldBasis[0][0] = East[0];
        sWorldBasis[0][1] = East[1];
        sWorldBasis[0][2] = East[2];
        // World +Y = Up
        sWorldBasis[1][0] = Up[0];
        sWorldBasis[1][1] = Up[1];
        sWorldBasis[1][2] = Up[2];
        // World +Z = South = -North
        sWorldBasis[2][0] = -North[0];
        sWorldBasis[2][1] = -North[1];
        sWorldBasis[2][2] = -North[2];
    }

    Wgs84::GeoPoint Wgs84::GetWorldOrigin()
    {
        return sOrigin;
    }

    QVector3D Wgs84::ToWorld(double LatDeg, double LonDeg, double AltMeters)
    {
        double X, Y, Z;
        GeodeticToEcef(LatDeg, LonDeg, AltMeters, X, Y, Z);

        const double dx = X - sOriginEcef[0];
        const double dy = Y - sOriginEcef[1];
        const double dz = Z - sOriginEcef[2];

        const double ecef[3] = { dx, dy, dz };
        return QVector3D(static_cast<float>(Dot(sWorldBasis[0], ecef)), static_cast<float>(Dot(sWorldBasis[1], ecef)), static_cast<float>(Dot(sWorldBasis[2], ecef)));
    }

    Wgs84::GeoPoint Wgs84::ToGeodetic(const QVector3D& WorldPos)
    {
        // Reconstruct ECEF from world coordinates
        const double wx = static_cast<double>(WorldPos.x());
        const double wy = static_cast<double>(WorldPos.y());
        const double wz = static_cast<double>(WorldPos.z());

        const double X = sOriginEcef[0] + wx * sWorldBasis[0][0] + wy * sWorldBasis[1][0] + wz * sWorldBasis[2][0];
        const double Y = sOriginEcef[1] + wx * sWorldBasis[0][1] + wy * sWorldBasis[1][1] + wz * sWorldBasis[2][1];
        const double Z = sOriginEcef[2] + wx * sWorldBasis[0][2] + wy * sWorldBasis[1][2] + wz * sWorldBasis[2][2];

        return EcefToGeodetic(X, Y, Z);
    }

    QVector3D Wgs84::EllipsoidNormal(double LatDeg, double LonDeg)
    {
        double Up[3];
        ComputeEnuEcef(LatDeg, LonDeg, nullptr, nullptr, Up);

        return QVector3D(static_cast<float>(Dot(sWorldBasis[0], Up)), static_cast<float>(Dot(sWorldBasis[1], Up)), static_cast<float>(Dot(sWorldBasis[2], Up)));
    }

    QMatrix3x3 Wgs84::EnuToWorld(double LatDeg, double LonDeg)
    {
        double E[3], N_[3], U[3];
        ComputeEnuEcef(LatDeg, LonDeg, E, N_, U);

        // Columns: East, North, Up in world space
        float vals[9];

        // Column 0 = East
        vals[0] = static_cast<float>(Dot(sWorldBasis[0], E));
        vals[3] = static_cast<float>(Dot(sWorldBasis[1], E));
        vals[6] = static_cast<float>(Dot(sWorldBasis[2], E));

        // Column 1 = North
        vals[1] = static_cast<float>(Dot(sWorldBasis[0], N_));
        vals[4] = static_cast<float>(Dot(sWorldBasis[1], N_));
        vals[7] = static_cast<float>(Dot(sWorldBasis[2], N_));

        // Column 2 = Up
        vals[2] = static_cast<float>(Dot(sWorldBasis[0], U));
        vals[5] = static_cast<float>(Dot(sWorldBasis[1], U));
        vals[8] = static_cast<float>(Dot(sWorldBasis[2], U));

        // QMatrix3x3 constructor reads row-major: vals[row*3 + col]
        // We have vals laid out as [col0row0, col1row0, col2row0, col0row1, col1row1, col2row1, ...]
        // which is exactly row-major (row 0 = [E.x, N.x, U.x] in world)
        return QMatrix3x3(vals);
    }

} // namespace Canavar::Engine
