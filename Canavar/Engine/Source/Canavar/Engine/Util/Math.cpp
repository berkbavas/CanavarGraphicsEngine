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

float Canavar::Engine::Math::SignedAngleBetween(const QVector3D& V1, const QVector3D& V2, const QVector3D& Axis)
{
    QVector3D A = V1.normalized();
    QVector3D B = V2.normalized();
    QVector3D N = Axis.normalized();

    float Dot = QVector3D::dotProduct(A, B);
    QVector3D Cross = QVector3D::crossProduct(A, B);
    float Sign = QVector3D::dotProduct(N, Cross);

    return qRadiansToDegrees(qAtan2(Sign, Dot));
}

QVector3D Canavar::Engine::Math::ProjectOntoPlane(const QVector3D& Vector, const QVector3D& PlaneNormal)
{
    return Vector - QVector3D::dotProduct(Vector, PlaneNormal) * PlaneNormal;
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

bool Canavar::Engine::Math::IntersectRayPlane(const QVector3D& RayOrigin, const QVector3D& RayDir, const QVector3D& PlanePoint, const QVector3D& PlaneNormal, QVector3D& IntersectionPoint)
{
    float Denom = QVector3D::dotProduct(RayDir, PlaneNormal);
    if (qFuzzyCompare(Denom, 0.0f))
    {
        // Ray is parallel to the plane
        return false;
    }

    float t = QVector3D::dotProduct(PlanePoint - RayOrigin, PlaneNormal) / Denom;
    if (t < 0)
    {
        // Intersection point is behind the ray origin
        return false;
    }

    IntersectionPoint = RayOrigin + t * RayDir;
    return true;
}

bool Canavar::Engine::Math::IntersectRaySphere(const QVector3D& RayOrigin, const QVector3D& RayDirection, const QVector3D& SphereCenter, float SphereRadius, QVector3D& OutIntersectionPoint)
{
    QVector3D L = SphereCenter - RayOrigin;
    float Dot = QVector3D::dotProduct(L, RayDirection);
    if (Dot < 0)
    {
        return false; // Sphere is behind the ray
    }

    float D2 = QVector3D::dotProduct(L, L) - Dot * Dot;
    float R2 = SphereRadius * SphereRadius;
    if (D2 > R2)
    {
        return false; // No intersection
    }

    float THC = sqrt(R2 - D2);
    float T0 = Dot - THC;
    float T1 = Dot + THC;

    // Use the closest intersection point
    float T = (T0 < T1) ? T0 : T1;
    OutIntersectionPoint = RayOrigin + T * RayDirection;
    return true;
}
