#include "Converter.h"

#include <QQuaternion>
#include <QtMath>

Canavar::Simulator::Converter::Converter(double RefLatitude, double RefLongitude, double RefAltitude)
    : mReferenceLatitude(RefLatitude)
    , mReferenceLongitude(RefLongitude)
    , mReferenceAltitude(RefAltitude)
{
    mReferencePosition = Canavar::Simulator::Converter::GeodeticToEcef(RefLatitude, RefLongitude, RefAltitude);
}

QVector3D Canavar::Simulator::Converter::ConvertPositionToCartesian(double Latitude, double Longitude, double Altitude)
{
    QQuaternion EcefToLocal = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), 90 - Latitude) * //
                              QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), Longitude);

    QVector3D Ecef = Canavar::Simulator::Converter::GeodeticToEcef(Latitude, Longitude, Altitude);
    QVector3D EcefDelta = EcefToLocal.inverted() * (Ecef - mReferencePosition);

    return QVector3D(EcefDelta.y(), EcefDelta.z(), EcefDelta.x());
}

QQuaternion Canavar::Simulator::Converter::ConvertRotation(double Latitude, double Longitude, const QQuaternion& LocalToBody)
{
    QQuaternion Fix = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), Latitude - mReferenceLatitude) * //
                      QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), Longitude - mReferenceLongitude);
                      
    QQuaternion fixed = Fix.inverted() * LocalToBody;

    float x;
    float y;
    float z;
    float angle;
    fixed.getAxisAndAngle(&x, &y, &z, &angle);

    return QQuaternion::fromAxisAndAngle(QVector3D(y, -z, -x), angle);
}

QVector3D Canavar::Simulator::Converter::GeodeticToEcef(double Latitude, double Longitude, double Altitude)
{
    double LatDegrees = qDegreesToRadians(Latitude);
    double Lon = qDegreesToRadians(Longitude);
    double H = Altitude;
    double NValue = N(LatDegrees);
    float X = (NValue + H) * cos(LatDegrees) * cos(Lon);
    float Y = (NValue + H) * cos(LatDegrees) * sin(Lon);
    float Z = (pow(SEMI_MINOR_RADIUS / SEMI_MAJOR_RADIUS, 2) * NValue + H) * sin(LatDegrees);

    return QVector3D(X, Y, Z);
}

double Canavar::Simulator::Converter::N(double Latitude)
{
    return pow(SEMI_MAJOR_RADIUS, 2) / std::sqrt(std::pow(SEMI_MAJOR_RADIUS * std::cos(Latitude), 2) + std::pow(SEMI_MINOR_RADIUS * std::sin(Latitude), 2));
}