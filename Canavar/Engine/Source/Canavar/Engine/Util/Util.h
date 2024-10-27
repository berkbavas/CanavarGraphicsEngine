#pragma once

#include <QByteArray>
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

      private:
        static QRandomGenerator mGenerator;
    };
}
