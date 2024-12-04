#include "Converter.h"

#include <QQuaternion>
#include <QtMath>

Canavar::Simulator::Converter::Converter(double latitude, double longitude, double altitude)
    : mReferenceLatitude(latitude)
    , mReferenceLongitude(longitude)
    , mReferenceAltitude(altitude)
{
    mReferencePosition = Canavar::Simulator::Converter::GeodeticToEcef(latitude, longitude, altitude);
}

QVector3D Canavar::Simulator::Converter::ConvertPositionToCartesian(double latitude, double longitude, double altitude)
{
    QQuaternion ecefToLocal = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), 90 - latitude) * //
                              QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), longitude);

    QVector3D ecef = Canavar::Simulator::Converter::GeodeticToEcef(latitude, longitude, altitude);
    QVector3D ecefDelta = ecefToLocal.inverted() * (ecef - mReferencePosition);

    return QVector3D(ecefDelta.y(), ecefDelta.z(), ecefDelta.x());
}

QQuaternion Canavar::Simulator::Converter::ConvertRotation(double latitude, double longitude, const QQuaternion& localToBody)
{
    QQuaternion fix = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), latitude - mReferenceLatitude) * //
                      QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), longitude - mReferenceLongitude);
    QQuaternion fixed = fix.inverted() * localToBody;

    float x;
    float y;
    float z;
    float angle;
    fixed.getAxisAndAngle(&x, &y, &z, &angle);

    return QQuaternion::fromAxisAndAngle(QVector3D(y, -z, -x), angle);
}

QVector3D Canavar::Simulator::Converter::GeodeticToEcef(double latitude, double longitude, double altitude)
{
    double lat = qDegreesToRadians(latitude);
    double lon = qDegreesToRadians(longitude);
    double h = altitude;
    double n = N(lat);
    float x = (n + h) * cos(lat) * cos(lon);
    float y = (n + h) * cos(lat) * sin(lon);
    float z = (pow(SEMI_MINOR_RADIUS / SEMI_MAJOR_RADIUS, 2) * n + h) * sin(lat);

    return QVector3D(x, y, z);
}

double Canavar::Simulator::Converter::N(double latitude)
{
    return pow(SEMI_MAJOR_RADIUS, 2) / sqrt(pow(SEMI_MAJOR_RADIUS * cos(latitude), 2) + pow(SEMI_MINOR_RADIUS * sin(latitude), 2));
}

const double Canavar::Simulator::Converter::SEMI_MAJOR_RADIUS = 6378137.0;
const double Canavar::Simulator::Converter::SEMI_MINOR_RADIUS = 6356752.314245;