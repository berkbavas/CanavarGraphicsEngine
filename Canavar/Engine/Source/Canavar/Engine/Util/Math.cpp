#include "Math.h"

void Canavar::Engine::Math::GetEulerDegrees(const QQuaternion& Rotation, float& Yaw, float& Pitch, float& Roll)
{
    QVector3D zAxis = Rotation * QVector3D(0, 0, -1);
    float x = zAxis.x();
    float y = zAxis.y();
    float z = zAxis.z();
    Yaw = qRadiansToDegrees(atan2(-z, x)) - 90;
    Pitch = qRadiansToDegrees(atan2(y, sqrt(z * z + x * x)));

    QVector3D xAxis = Rotation * QVector3D(1, 0, 0);

    QVector3D xAxisProj = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), Yaw) *   //
                          QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), Pitch) * //
                          QVector3D(1, 0, 0);

    QVector3D left = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), Yaw) *   //
                     QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), Pitch) * //
                     QVector3D(0, 0, -1);

    Roll = Canavar::Engine::Math::AngleBetween(xAxis, xAxisProj, left);

    if (Yaw < 0.0f)
    {
        Yaw += 360.0f;
    }
}

QQuaternion Canavar::Engine::Math::ConstructFromEulerDegrees(float Yaw, float Pitch, float Roll)
{
    return QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), Yaw) *   //
           QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), Pitch) * //
           QQuaternion::fromAxisAndAngle(QVector3D(0, 0, -1), Roll);
}

float Canavar::Engine::Math::AngleBetween(const QVector3D& V1, const QVector3D& V2, const QVector3D& Left)
{
    QVector3D U1 = V1.normalized();
    QVector3D U2 = V2.normalized();

    float Dot = QVector3D::dotProduct(U1, U2);

    if (qFuzzyCompare(Dot, 1.0f))
    {
        return 0.0f;
    }
    else if (qFuzzyCompare(Dot, -1.0f))
    {
        return 180.0f;
    }
    else
    {
        float Angle = qRadiansToDegrees(acos(Dot));
        QVector3D U1XU2 = QVector3D::crossProduct(U1, U2);

        if (QVector3D::dotProduct(U1XU2, Left) < 0.0001f)
            return Angle;
        else
            return -Angle;
    }
}

float Canavar::Engine::Math::AngleBetween(const QVector3D& V1, const QVector3D& V2)
{
    QVector3D U1 = V1.normalized();
    QVector3D U2 = V2.normalized();

    float Dot = QVector3D::dotProduct(U1, U2);

    if (qFuzzyCompare(Dot, 1.0f))
    {
        return 0.0f;
    }
    else if (qFuzzyCompare(Dot, -1.0f))
    {
        return 180.0f;
    }
    else
    {
        return qRadiansToDegrees(acos(Dot));
    }
}

QQuaternion Canavar::Engine::Math::RotationBetweenVectors(const QVector3D& V0, const QVector3D& V1)
{
    QVector3D From = V0.normalized();
    QVector3D To = V1.normalized();

    float Dot = QVector3D::dotProduct(From, To);

    // If vectors are almost the same
    if (qFuzzyCompare(Dot, 1.0f))
    {
        return QQuaternion(); // Identity
    }

    // If vectors are opposite
    if (qFuzzyCompare(Dot, -1.0f))
    {
        // Find an orthogonal vector to use as the rotation axis
        QVector3D Orthogonal = QVector3D(1, 0, 0).crossProduct(From, QVector3D(1, 0, 0));
        if (Orthogonal.lengthSquared() < 1e-6)
        {
            Orthogonal = QVector3D(0, 1, 0).crossProduct(From, QVector3D(0, 1, 0));
        }
        Orthogonal.normalize();
        return QQuaternion::fromAxisAndAngle(Orthogonal, 180.0f);
    }

    QVector3D Axis = QVector3D::crossProduct(From, To);
    float Angle = qRadiansToDegrees(std::acos(Dot));

    return QQuaternion::fromAxisAndAngle(Axis.normalized(), Angle);
}
