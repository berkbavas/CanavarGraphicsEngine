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

        static QByteArray GetBytes(const QString& path);
        static QVector3D GenerateRandomVector(float x, float y, float z);
        static float GenerateRandom(float bound);

        static QString GenerateUuid();
        static bool WriteTextToFile(const QString& path, const QByteArray& content);
        static bool WriteDataToFile(const QString& path, const QByteArray& content);
        static QByteArray ReadDataFromFile(const QString& path);
        static QJsonDocument ReadJson(const QString& path);

      private:
        static QRandomGenerator mGenerator;
    };
}
