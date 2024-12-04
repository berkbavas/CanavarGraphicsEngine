#include "Math.h"

void Canavar::Engine::Math::GetEulerDegrees(const QQuaternion& rotation, float& yaw, float& pitch, float& roll)
{
    QVector3D zAxis = rotation * QVector3D(0, 0, -1);
    float x = zAxis.x();
    float y = zAxis.y();
    float z = zAxis.z();
    yaw = qRadiansToDegrees(atan2(-z, x)) - 90;
    pitch = qRadiansToDegrees(atan2(y, sqrt(z * z + x * x)));

    QVector3D xAxis = rotation * QVector3D(1, 0, 0);

    QVector3D xAxisProj = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), yaw) *   //
                          QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), pitch) * //
                          QVector3D(1, 0, 0);

    QVector3D left = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), yaw) *   //
                     QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), pitch) * //
                     QVector3D(0, 0, -1);

    roll = Canavar::Engine::Math::AngleBetween(xAxis, xAxisProj, left);

    if (yaw < 0.0f)
    {
        yaw += 360.0f;
    }
}

QQuaternion Canavar::Engine::Math::ConstructFromEulerDegrees(float yaw, float pitch, float roll)
{
    return QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), yaw) *   //
           QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), pitch) * //
           QQuaternion::fromAxisAndAngle(QVector3D(0, 0, -1), roll);
}

float Canavar::Engine::Math::AngleBetween(const QVector3D& v1, const QVector3D& v2, const QVector3D& left)
{
    QVector3D u1 = v1.normalized();
    QVector3D u2 = v2.normalized();

    float dot = QVector3D::dotProduct(u1, u2);

    if (qFuzzyCompare(dot, 1.0f))
    {
        return 0.0f;
    }
    else if (qFuzzyCompare(dot, -1.0f))
    {
        return 180.0f;
    }
    else
    {
        float angle = qRadiansToDegrees(acos(dot));
        QVector3D u1xu2 = QVector3D::crossProduct(u1, u2);

        if (QVector3D::dotProduct(u1xu2, left) < 0.0001f)
            return angle;
        else
            return -angle;
    }
}

float Canavar::Engine::Math::AngleBetween(const QVector3D& v1, const QVector3D& v2)
{
    QVector3D u1 = v1.normalized();
    QVector3D u2 = v2.normalized();

    float dot = QVector3D::dotProduct(u1, u2);

    if (qFuzzyCompare(dot, 1.0f))
    {
        return 0.0f;
    }
    else if (qFuzzyCompare(dot, -1.0f))
    {
        return 180.0f;
    }
    else
    {
        return qRadiansToDegrees(acos(dot));
    }
}
