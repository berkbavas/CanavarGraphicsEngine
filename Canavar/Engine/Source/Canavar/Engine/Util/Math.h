#pragma once

#include <QByteArray>
#include <QQuaternion>
#include <QString>

namespace Canavar::Engine
{
    class Math
    {
      public:
        Math() = delete;

        static void GetEulerDegrees(const QQuaternion& Rotation, float& Yaw, float& Pitch, float& Roll);
        static QQuaternion ConstructFromEulerDegrees(float Yaw, float Pitch, float Roll);

        static float AngleBetween(const QVector3D& V1, const QVector3D& V2, const QVector3D& Left);
        static float AngleBetween(const QVector3D& V1, const QVector3D& V2);

        static QQuaternion RotationBetweenVectors(const QVector3D& From, const QVector3D& To);

        template<typename T>
        static void AddIfLess(T& Value, T Bound, T Increment)
        {
            if (Value < Bound)
            {
                Value += Increment;
            }
        }

        template<typename T>
        static void AddIfGreater(T& Value, T Bound, T Increment)
        {
            if (Value > Bound)
            {
                Value += Increment;
            }
        }
    };
}
