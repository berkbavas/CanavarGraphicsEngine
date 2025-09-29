#include "Util.h"

#include "Canavar/Engine/Util/Logger.h"

#include <QFile>
#include <QUuid>

QByteArray Canavar::Engine::Util::GetBytes(const QString& Path)
{
    QFile File(Path);

    if (File.open(QFile::ReadOnly))
    {
        return File.readAll();
    }
    else
    {
        LOG_WARN("Util::GetBytes: '{}' could not be opened.", Path.toStdString());
        return QByteArray();
    }
}

QVector3D Canavar::Engine::Util::GenerateRandomVector(float x, float y, float z)
{
    return QVector3D(GenerateRandom(x), GenerateRandom(y), GenerateRandom(z));
}

float Canavar::Engine::Util::GenerateRandom(float Bound)
{
    return mGenerator.bounded(Bound);
}

QString Canavar::Engine::Util::GenerateUuid()
{
    return QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces);
}

bool Canavar::Engine::Util::WriteTextToFile(const QString& Path, const QByteArray& Content)
{
    QFile File(Path);

    if (File.open(QIODevice::WriteOnly))
    {
        QTextStream Stream(&File);
        Stream.setEncoding(QStringConverter::Utf8);
        Stream << Content;
        Stream.flush();
        File.close();
        return true;
    }
    else
    {
        LOG_FATAL("Util::WriteTextToFile: Could not write to file: {}", Path.toStdString());
        return false;
    }
}

bool Canavar::Engine::Util::WriteDataToFile(const QString& Path, const QByteArray& Content)
{
    QFile File(Path);
    if (File.open(QIODevice::WriteOnly))
    {
        QDataStream Stream(&File);
        Stream << Content;
        File.close();
        return true;
    }
    else
    {
        LOG_FATAL("Util::WriteDataToFile: Could not write to file: {}", Path.toStdString());
        return false;
    }
}

QByteArray Canavar::Engine::Util::ReadDataFromFile(const QString& Path)
{
    QFile File(Path);
    if (File.open(QIODevice::ReadOnly))
    {
        QByteArray Array;
        QDataStream Stream(&File);
        Stream >> Array;
        File.close();
        return Array;
    }
    else
    {
        LOG_FATAL("Util::ReadDataFromFile: Could not read from file: {}", Path.toStdString());
        return QByteArray();
    }
}

QJsonDocument Canavar::Engine::Util::ReadJson(const QString& Path)
{
    QJsonDocument Document;

    QFile File(Path);
    if (!File.open(QIODevice::ReadOnly))
    {
        LOG_FATAL("Util::ReadJson: Error occured while reading the file: {}", Path.toStdString());
        return QJsonDocument();
    }

    Document = QJsonDocument::fromJson(File.readAll());
    File.close();

    return Document;
}

QRandomGenerator Canavar::Engine::Util::mGenerator = QRandomGenerator::securelySeeded();
