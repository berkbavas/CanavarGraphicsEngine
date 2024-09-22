#pragma once

#include <QQuaternion>
#include <QVector3D>

// Sources:
// https://en.wikipedia.org/wiki/Geographic_coordinate_conversion#From_geodetic_to_ECEF_coordinates
// https://en.wikipedia.org/wiki/World_Geodetic_System

class Converter
{
public:
    Converter(double latitude, double longitude, double altitude);
    QVector3D ToOpenGL(double latitude, double longitude, double altitude);
    QQuaternion ToOpenGL(double latitude, double longitude, const QQuaternion& localToBody);

    static QVector3D GeodeticToEcef(double latitude, double longitude, double altitude);
    static double N(double latitude);

private:
    double mReferenceLatitude;
    double mReferenceLongitude;
    double mReferenceAltitude;
    QVector3D mReferencePosition;

    static const double SEMI_MAJOR_RADIUS; // a
    static const double SEMI_MINOR_RADIUS; // b
};
