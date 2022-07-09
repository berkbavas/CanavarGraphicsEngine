#include "Helper.h"

#include <QDebug>
#include <QFile>

Helper::Helper() {}

QByteArray Helper::getBytes(QString path)
{
    QFile file(path);
    if (file.open(QFile::ReadOnly))
    {
        return file.readAll();
    }
    else
    {
        qWarning() << QString("Could not open '%1'").arg(path);
        return QByteArray();
    }
}
