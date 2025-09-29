#pragma once

#include <QByteArray>
#include <QJsonDocument>
#include <QRandomGenerator>
#include <QString>
#include <QVector3D>

namespace Canavar::Engine
{
    class Util
    {
      public:
        Util() = delete;

        static QByteArray GetBytes(const QString& Path);
        static QVector3D GenerateRandomVector(float X, float Y, float Z);
        static float GenerateRandom(float Bound);

        static QString GenerateUuid();
        static bool WriteTextToFile(const QString& Path, const QByteArray& Content);
        static bool WriteDataToFile(const QString& Path, const QByteArray& Content);
        static QByteArray ReadDataFromFile(const QString& Path);
        static QJsonDocument ReadJson(const QString& Path);

        template<typename T, typename S>
        static bool Contains(const std::vector<T>& Vector, const S& Value)
        {
            return std::find(Vector.begin(), Vector.end(), std::dynamic_pointer_cast<T>(Value)) != Vector.end();
        }

      private:
        static QRandomGenerator mGenerator;
    };
}
