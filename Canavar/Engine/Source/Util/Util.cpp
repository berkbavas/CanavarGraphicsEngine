#include "Util.h"

#include "Util/Logger.h"

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