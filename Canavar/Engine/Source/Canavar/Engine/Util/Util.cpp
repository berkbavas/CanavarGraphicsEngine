#include "Util.h"

#include "Canavar/Engine/Util/Logger.h"

#include <QFile>

QByteArray Canavar::Engine::Util::GetBytes(const QString& path)
{
    QFile file(path);

    if (file.open(QFile::ReadOnly))
    {
        return file.readAll();
    }
    else
    {
        LOG_WARN("Util::GetBytes: '{}' could not be opened.", path.toStdString());
        return QByteArray();
    }
}

QVector3D Canavar::Engine::Util::GenerateRandomVector(float x, float y, float z)
{
    return QVector3D(GenerateRandom(x), GenerateRandom(y), GenerateRandom(z));
}

float Canavar::Engine::Util::GenerateRandom(float bound)
{
    return mGenerator.bounded(bound);
}

QRandomGenerator Canavar::Engine::Util::mGenerator = QRandomGenerator::securelySeeded();
