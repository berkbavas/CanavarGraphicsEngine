#pragma once

#include <QQuaternion>
#include <QVector3D>

// Sources:
// https://en.wikipedia.org/wiki/Geographic_coordinate_conversion#From_geodetic_to_ECEF_coordinates
// https://en.wikipedia.org/wiki/World_Geodetic_System

namespace Canavar::Simulator
{
    class Converter
    {
      public:
        Converter(double RefLat, double RefLon, double RefAlt);
        QVector3D ConvertPositionToCartesian(double Latitude, double Longitude, double Altitude);
        QQuaternion ConvertRotation(double Latitude, double Longitude, const QQuaternion& LocalToBody);

        static QVector3D GeodeticToEcef(double Latitude, double Longitude, double Altitude);
        static double N(double Latitude);

      private:
        double mReferenceLatitude;
        double mReferenceLongitude;
        double mReferenceAltitude;
        QVector3D mReferencePosition;

        static constexpr double SEMI_MAJOR_RADIUS = 6378137.0;      // A
        static constexpr double SEMI_MINOR_RADIUS = 6356752.314245; // B
    };
}