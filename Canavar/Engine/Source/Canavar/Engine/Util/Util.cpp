#include "Util.h"

#include "Canavar/Engine/Util/Logger.h"

#include <QFile>
#include <QUuid>

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

QString Canavar::Engine::Util::GenerateUuid()
{
    return QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces);
}

bool Canavar::Engine::Util::WriteTextToFile(const QString& path, const QByteArray& content)
{
    QFile file(path);

    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);
        stream.setEncoding(QStringConverter::Utf8);
        stream << content;
        stream.flush();
        file.close();
        return true;
    }
    else
    {
        LOG_FATAL("Util::WriteTextToFile: Could not write to file: {}", path.toStdString());
        return false;
    }
}

bool Canavar::Engine::Util::WriteDataToFile(const QString& path, const QByteArray& content)
{
    QFile file(path);
    if (file.open(QIODevice::WriteOnly))
    {
        QDataStream stream(&file);
        stream << content;
        file.close();
        return true;
    }
    else
    {
        LOG_FATAL("Util::WriteDataToFile: Could not write to file: {}", path.toStdString());
        return false;
    }
}

QByteArray Canavar::Engine::Util::ReadDataFromFile(const QString& path)
{
    QFile file(path);
    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray arr;
        QDataStream stream(&file);
        stream >> arr;
        file.close();
        return arr;
    }
    else
    {
        LOG_FATAL("Util::ReadDataFromFile: Could not read from file: {}", path.toStdString());
        return QByteArray();
    }
}

QJsonDocument Canavar::Engine::Util::ReadJson(const QString& path)
{
    QJsonDocument document;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        LOG_FATAL("Util::ReadJson: Error occured while reading the file: {}", path.toStdString());
        return QJsonDocument();
    }

    document = QJsonDocument::fromJson(file.readAll());
    file.close();

    return document;
}

QRandomGenerator Canavar::Engine::Util::mGenerator = QRandomGenerator::securelySeeded();
