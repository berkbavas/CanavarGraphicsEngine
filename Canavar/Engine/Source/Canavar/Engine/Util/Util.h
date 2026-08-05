#pragma once

#include <QByteArray>
#include <QJsonDocument>
#include <QOpenGLExtraFunctions>
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
        static bool WriteTextToFile(const QString& Path, const QByteArray& Content);
        static bool WriteDataToFile(const QString& Path, const QByteArray& Content);
        static QByteArray ReadDataFromFile(const QString& Path);
        static QJsonDocument ReadJson(const QString& Path);
        static const char* GetGlErrorString(GLenum Error);

      private:
        static QRandomGenerator sGenerator;
    };
}
